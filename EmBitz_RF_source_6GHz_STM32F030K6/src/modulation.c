#include "stm32f0xx.h"
#include "system_stm32f0xx.h"
#include <stdio.h>
#include <stdlib.h>

#include "pinout.h"

int denominator = 1;
int numerator = 0;
int denominator_i = 0; // Internal calculation register

/*
TO-DO ulomek lahko izvaja dodaten stevec, ki je povezan na triger TIM3 - potem je kode prekinitvenih rutin prakticno nic
*/

void timer3_init(void){
    RCC->APB1ENR |= (RCC_APB1ENR_PWREN | RCC_APB1ENR_TIM3EN); // Clock enable

    TIM3->CR1 |= TIM_CR1_ARPE | TIM_CR1_DIR; // Preload register has its own buffer, counting down
    TIM3->SMCR = 0; // Slave mode off
    TIM3->DIER |= TIM_DIER_UIE; // Enable interrupt on counter reset
    TIM3->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // PWM mode 1 - In downcounting, channel 1 is inactive (OC1REF=‘0) as long as TIMx_CNT>TIMx_CCR1 else active (OC1REF=1).
    TIM3->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE; // Preload register on TIMx_CCR1 enabled. Read/Write operations access the preload register. TIMx_CCR1 preload value is loaded in the active register at each update event
    TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E; // On - OC1 signal is output on the corresponding output pin

    TIM3->CNT = 0; // Reset counter
    TIM3->PSC = 0; // Pre-scaler value
    TIM3->ARR = 0xFFFFFFFF; // 0 value is blocking! - Counter overflow/underflow gets register updates

    TIM3->CCR1 = 0;
    TIM3->CCR2 = 0;

    TIM3->EGR = TIM_EGR_UG; // Re-initializes the timer counter

    NVIC_DisableIRQ(TIM3_IRQn); // Disable TIM3 IRQ
    NVIC_SetPriority(TIM3_IRQn,1);   //enable interrupt

    //TIM3->SR & TIM_SR_CC3IF;
}

void modulationSet(int mod_freq){
    // Set counter and enable modulation
    /*
    Tu napisemo nekoliko vec kode da nastavimo ustrezen modulo deljenja in tega ne prepuscamo samodejni
    odlocitvi programske kode. V praksi se izkaze da je bolje, ce modula ne spreminjamo konstantno saj tako
    dobimo bolj zaneslivo delovanje
    */
    // change AF1 output mode
    MOD_LOW;
    RFEN_LOW;
    GPIOA->MODER &= (~GPIO_MODER_MODER6) | GPIO_MODER_MODER6_1; // MOD output - AF1
    GPIOB->MODER &= (~GPIO_MODER_MODER5) | GPIO_MODER_MODER5_1;; // MOD output - AF1

    if(mod_freq <= 0){
        // Stop the counter
        TIM3->CR1 &= ~TIM_CR1_CEN; // TIMER OFF
        TIM3->EGR = TIM_EGR_UG; // Re-initializes the timer counter
        NVIC_DisableIRQ(TIM3_IRQn); // Disable interrupt

        // set pin to output mode
        GPIOA->MODER &= (~GPIO_MODER_MODER6) | GPIO_MODER_MODER6_0; // MOD output - normal output
        GPIOB->MODER &= (~GPIO_MODER_MODER5) | GPIO_MODER_MODER5_0;; // MOD output - normal output
        MOD_HIGH;
        RFEN_HIGH;
    }
    else if(mod_freq < 100){
        // Greater resolution under 100 Hz
        // Divide clock by 960 - reference now 50 kHz

        // Calculate the counter needed
        int N_counter = 50000/mod_freq;
        numerator = 50000%mod_freq;
        denominator = mod_freq;
        denominator_i = denominator;

        TIM3->CCR1 = N_counter/2;
        TIM3->CCR2 = N_counter/2; // half period
        TIM3->ARR = N_counter-1; // full period
        TIM3->PSC = 960-1; // The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1).

        // vklopimo stevec
        TIM3->EGR = TIM_EGR_UG; // Re-initializes the timer counter
        TIM3->CR1 |= TIM_CR1_CEN; // TIMER ON
        NVIC_EnableIRQ(TIM3_IRQn); // enable interrupt
    }
    else if(mod_freq <= 1000){
        // If under 1 kHz we need to modify modulo value and calculating factors
        // divide by 8, reference clock is therefore 6 MHz

        // Calculate the counter needed, rounded down
        int N_counter = 6000000/mod_freq;
        numerator = 6000000%mod_freq;
        denominator = mod_freq;
        denominator_i = denominator;

        TIM3->CCR1 = N_counter/2;
        TIM3->CCR2 = N_counter/2; // half period
        TIM3->ARR = N_counter-1; // full period

        TIM3->PSC = 8-1; // The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1).

        // vklopimo stevec
        TIM3->EGR = TIM_EGR_UG; // Re-initializes the timer counter
        TIM3->CR1 |= TIM_CR1_CEN; // TIMER ON
        NVIC_EnableIRQ(TIM3_IRQn); // enable interrupts
    }
    else{
         // Calculate the counter needed, rounded down
        int N_counter = 48000000/mod_freq;
        numerator = 48000000%mod_freq;
        denominator = mod_freq;
        denominator_i = denominator;

        TIM3->CCR1 = N_counter/2;
        TIM3->CCR2 = N_counter/2;
        TIM3->ARR = N_counter-1;

        TIM3->PSC = 0;

        // vklopimo stevec
        TIM3->EGR = TIM_EGR_UG; // Re-initializes the timer counter
        TIM3->CR1 |= TIM_CR1_CEN; // TIMER ON
        NVIC_EnableIRQ(TIM3_IRQn); // enable interrupt
    }
}

void TIM3_IRQHandler(void){
    // TIM3 Interrupt handler
    TIM3->SR = 0; // clear all flags, other unused.

    // re-calculate registers
    if(numerator){
        if(denominator_i){
            if(denominator_i == numerator){
                TIM3->ARR++; // add 1 to compare value
            }
            denominator_i--;
        }
        else{
            denominator_i = denominator; // insert new counter value
            TIM3->ARR--; // lower it to the original value
        }
    }
}
