#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f0xx.h"
#include "system_stm32f0xx.h"

#include "pinout.h"
#include "delay.h"
#include "usart.h"
#include "menu.h"

#include "max2871.h"
#include "display.h"
#include "attenuator.h"

#include "calibration.h"
#include "modulation.h"
#include "flash.h"

// Values at Flash memory
__attribute__((__section__(".mem"))) const int program_flash_memory[10][3] = {
{6000000,0,0},
{6000000,0,0},
{6000000,0,0},
{6000000,0,0},
{6000000,0,0},
{6000000,0,0},
{6000000,0,0},
{6000000,0,0},
{6000000,0,0},
{6000000,0,0}
};

// primitives
void init(void);

// USART - variables for serial communication
char buffer[20]; //max 20 characters
int buff_index = 0;
int decode_command = 0;

// RAM memory - 0 - frequency, 1 - output power, 2 - memory
int memory[10][3];

// Global variables - current values!
int cursor = 0;
int program = 0;
char status = '?';
int status_change = 0;
int frequency = 0; // in kHz
int frequency_output = 0;
int frequency_flash = 0;
int power = 0; // dBm
int modulation = 0; // Hz

int main(void)
{
    init();
    RFEN_LOW; // Output off as soon as possible
    timer14_init();
    timer16_init();
    lcdInit();

    // Write on display
    lcdDisplayClear();
    lcdWriteString("RF source 6 GHz \r\n Aljaz B. v2.1 ");
    lcdPositionXY(17,0); // remove cursor from visible area of the LCD screen

    // Init. everything else
    usart_init();
    attenuator_init();
    timer3_init();
    MAX2871_init();
    flashCopyFromFlashToRAM(); // Copy from Flash to RAM
    menuLoadDataFromMemoryToWorkingRAM(0); // Start at 0 (first/default program)

    delay_ms(3000); // 3S delay for boot flash screen
    // Display main menu
    lcdDisplayClear();
    menuShow();

    // Send report to USART
    usart_send_string("RF source 6 GHz\r\n");
    usart_send_string("v2.1\r\n");
    usart_send_report();

    SysTick_Config(4800000); // 100 mS config - status update rate


    while(1){
        moveCursorUD();
        moveCursorLR();

        if(decode_command){
            if(command_decode(buffer)){
                menuShow(); // ce je uspesno dekodiran ukaz, posodobimo menu in nastavitve
            }
            decode_command = 0;
        }

        if(status_change) {
            menuShowStatus(status);
            status_change = 0;
        }
        if(frequency != frequency_output){
            // desired an actual frequency are not the same
            if(frequency_flash>0){
                // display actual frequency
                frequency_flash--;
                if(!frequency_flash){
                    // if reached 0 change the value
                    menuShowFrequency(frequency);
                    frequency_flash = -100;  // 1s
                }
            }
            else if(frequency_flash<0){
                // display desired frequency
                frequency_flash++;
                if(!frequency_flash){
                    // if reached 0 change the value
                    menuShowFrequency(frequency_output);
                    frequency_flash = 300; // 3s
                }
            }
            else{
                // if not at 0, start from the beginning
                frequency_flash = 300; // 3s
            }
        }
        delay_ms(10);
    }
}


void init(void){
    RCC->CR |= RCC_CR_HSEON; // Turn HSE oscillator on
    while(!(RCC->CR & RCC_CR_HSERDY)); // Wait to be turned ON
    // TO-DO preveri ce je zunanji oscilator zagnan, drugace delaj na notranjega!
    // set frequency at 48 MHz
    RCC->CFGR &= ~RCC_CFGR_SW; // select internal clock again
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI); // wait for the switch to complete (to internal)
    RCC->CR &= ~RCC_CR_PLLON; // disable PLL
    while((RCC->CR & RCC_CR_PLLRDY) != 0); // wait for the command to execute
    RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_PLLMUL)) | RCC_CFGR_PLLMUL12; // PLL HSE/6 x 12
    RCC->CFGR2 = RCC_CFGR2_PREDIV_DIV6; // Divide HSE with 6 to get 4 MHz internal clock, same as HSI
    RCC->CFGR |= RCC_CFGR_PLLSRC; // HSE to PLL input
    RCC->CR |= RCC_CR_PLLON; // enable PLL
    while((RCC->CR & RCC_CR_PLLRDY) == 0); // wait for the PLL to turn ON
    RCC->CFGR |= RCC_CFGR_SW_PLL; // Do not wait for the PLL to be stable
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // Wait for a successive switch

    RCC->AHBENR |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN);  // Enable GPIO clock
    GPIOA->OSPEEDR |= 0xC3FFFFFF; // Full speed (leave PA13 and PA14 as is)
    GPIOB->OSPEEDR |= 0xFFFFFFFF; // Full speed

    // BUTTONS
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR8_0 | GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0 | GPIO_PUPDR_PUPDR11_0; // Pull_up

    // PLL - MAX2871
    GPIOA->MODER |= GPIO_MODER_MODER0_0; // General purpose output mode
    GPIOB->MODER |= (GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0); // General purpose output mode
    GPIOA->MODER |= GPIO_MODER_MODER6_1; // MOD output - MAX - ALTERNETE FUNCTION
    GPIOA->AFR[0] |= 0x01000000; // AF1

    // ATTEN - F1958
    GPIOA->MODER |= (GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 | GPIO_MODER_MODER7_0); // General purpose output mode

    // LCD
    GPIOA->MODER |= (GPIO_MODER_MODER12_0 | GPIO_MODER_MODER15_0); // General purpose output mode
    GPIOB->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0); // General purpose output mode

    // MOD
    GPIOB->MODER |= GPIO_MODER_MODER5_1; // MOD output - OUT - ALTERNETE FUNCTION
    GPIOB->AFR[0] |= 0x00100000; // AF1

    // RS-232
    GPIOA->MODER |= (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1); // Pin setup PA2 - TX | PA3 - RX - alternate function mode
    GPIOA->AFR[0] |= 0x00001100; // Alternate function 1 on pin PA2 and PA3
}

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

void NMI_Handler(void);
void HardFault_Handler(void){
    while (1);
}
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void){
    if(MUX){
        if(status == '?') status_change = 1;
        status = ' ';
    }
    else{
        if(status == ' ') status_change = 1;
        status = '?';
    }
}

void USART1_IRQHandler(void){
    if(USART1->ISR & USART_ISR_RXNE){ // new data in
        unsigned char data = USART1->RDR;
        if((data == 8 || data == 127) && (buff_index > 0)){
            buff_index--;
            USART1->TDR = data;
            return;
        }
        else if((data == 8 || data == 127) && buff_index<=0){
            return;
        }
        if(data>=32 || data == 13){
            //USART1->TDR = data; // echo
            buffer[buff_index] = data;
            if(data == 13){
                buffer[buff_index] = 0;
                decode_command = 1;
            }
            else{
                buff_index++;
            }
        }
    }
    else if(USART1->ISR & USART_ISR_ORE){ // overrun error - do nothing
        USART1->ICR |= USART_ICR_ORECF; // clear overrun error
    }
}
