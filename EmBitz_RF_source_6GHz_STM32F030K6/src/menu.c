#include "stm32f0xx.h"
#include "system_stm32f0xx.h"

#include "pinout.h"
#include "delay.h"
#include "usart.h"
#include "display.h"
#include "calibration.h"
#include "modulation.h"
#include "flash.h"

#include "max2871.h"
#include "attenuator.h"

extern int memory[10][3];

extern int cursor;
extern  int program;
extern  char status;
extern  int frequency; // kHz
extern  int frequency_output;
extern int frequency_flash;
extern int power; // dBm
extern  int modulation; // Hz

void menuLoadDataFromMemoryToWorkingRAM(int program){
    frequency = memory[program][0]; // 0 - frequency
    power = memory[program][1]; // 1 - power
    modulation = memory[program][2]; // 2 - modulation
}

void menuSaveDataFromWorkingRAMToMemory(int program){
    memory[program][0] = frequency; // 0 - frequency
    memory[program][1] = power; // 1 - power
    memory[program][2] = modulation; // 2 - modulation
}

void menuShow(void){
    lcdPositionXY(0,0);
    lcdWriteChar((unsigned char)(0x30 + program));
    lcdWriteChar(' ');
    lcdWriteChar(' ');
    // skip the status area - generated in the mail loop
    lcdPositionXY(4,0);
    lcdWriteChar(' ');

    // frequency
    int temp = frequency;
    char digit = 0x30;
    while((temp-1000000)>=0){
        digit++;
        temp = temp-1000000;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-100000)>=0){
        digit++;
        temp = temp-100000;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-10000)>=0){
        digit++;
        temp = temp-10000;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-1000)>=0){
        digit++;
        temp = temp-1000;
    }
    lcdWriteChar(digit);
    lcdWriteChar('.');
    digit = 0x30;
    while((temp-100)>=0){
        digit++;
        temp = temp-100;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-10)>=0){
        digit++;
        temp = temp-10;
    }
    lcdWriteChar(digit);
    digit = 0x30 + temp;
    lcdWriteChar(digit);
    lcdWriteString("MHz\n");

    // power
    if(!(power/10)){
       // skip the firs char if power -9<0<9
        lcdWriteChar(' ');
    }
    if(power>=0){
        lcdWriteChar('+');
        temp = power;
    }
    else{
        lcdWriteChar('-');
        temp = power + (-2*power);
    }
    digit = 0x30;
    while((temp-10)>=0){
        digit++;
        temp = temp-10;
    }
    if(digit != 0x30) lcdWriteChar(digit); // write only in -9<x>9
    digit = 0x30 + temp;
    lcdWriteChar(digit);
    lcdWriteString("dBm ");

    // modulation
    temp = modulation;
    if(!modulation){
        lcdWriteString("SW.OFF");
    }
    else{
        digit = 0x30;
        while((temp-10000)>=0){
            digit++;
            temp = temp-10000;
        }
        lcdWriteChar(digit);
        digit = 0x30;
        while((temp-1000)>=0){
            digit++;
            temp = temp-1000;
        }
        lcdWriteChar(digit);
        lcdWriteChar('.');
        digit = 0x30;
        while((temp-100)>=0){
            digit++;
            temp = temp-100;
        }
        lcdWriteChar(digit);
        digit = 0x30;
        while((temp-10)>=0){
            digit++;
            temp = temp-10;
        }
        lcdWriteChar(digit);
        digit = 0x30 + temp;
        lcdWriteChar(digit);
    }
    if(modulation){
         lcdWriteString("kHz");
    }
    else{
         lcdWriteString("   ");
    }
    lcdPositionXY(cursor%16,cursor/16);

    // Send update to ICs
    frequency_output = MAX2871_set_frequency(frequency);
    attenuator_send(calibrationReturnAttenuation(frequency,power));
    modulationSet(modulation);

    menuSaveDataFromWorkingRAMToMemory(program); // save updates to current memory RAm
    frequency_flash = -200;  // 2s frequency flash
}

void menuShowStatus(char status){
    lcdPositionXY(3,0);
    lcdWriteChar(status);
    lcdPositionXY(cursor%16,cursor/16);
}

void menuShowFrequency(int new_frequency){
    // update only frequency
    lcdPositionXY(5,0);

    int temp = new_frequency;
    char digit = 0x30;
    while((temp-1000000)>=0){
        digit++;
        temp = temp-1000000;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-100000)>=0){
        digit++;
        temp = temp-100000;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-10000)>=0){
        digit++;
        temp = temp-10000;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-1000)>=0){
        digit++;
        temp = temp-1000;
    }
    lcdWriteChar(digit);
    lcdWriteChar('.');
    digit = 0x30;
    while((temp-100)>=0){
        digit++;
        temp = temp-100;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-10)>=0){
        digit++;
        temp = temp-10;
    }
    lcdWriteChar(digit);
    digit = 0x30 + temp;
    lcdWriteChar(digit);
    // return cursor
    lcdPositionXY(cursor%16,cursor/16);
}

void moveCursorLR(){
    int cursor_temp = cursor; // save cursor position
    if(!bRIGHT){
        cursor++;
        if(cursor>0 && cursor<5){
            cursor = 5; // frequency
        }
        if(cursor == 9){
            cursor = 10;
        }
        if(cursor>12 && cursor<17){
            cursor = 17; // power
        }
        if(cursor>18 && cursor<23){
            cursor = 23; // modulation
        }
        if(cursor == 25){
            cursor = 26;
        }
        if(cursor>28){
            cursor = 28;
        }
        lcdPositionXY(cursor%16,cursor/16); // instant feedback to user
        int timer = 200;
        while(!bRIGHT && timer){
            delay_ms(1);
            if(!bLEFT){
                // user holds both buttons
                cursor = cursor_temp; // save cursor position
                lcdPositionXY(0,0); // set cursor to starting point
                lcdWriteString("**** SAVING ****\n");
                if(flashWriteChanges()){ // save changes to flash
                    lcdWriteString("  Please wait!  ");
                }
                else{
                    lcdWriteString("   No changes   ");
                }
                delay_ms(3000);
                lcdDisplayClear(); // clear up display
                timer = 0;
                menuShow(); // show back menu
                while(!bLEFT || !bRIGHT); // wait for user to release both buttons
            }
            timer--;
        }
        delay_ms(10); // de-bounce
    }
    if(!bLEFT){
        cursor--;
        if(cursor<0){
            cursor = 0;
        }
        if(cursor>0 && cursor<5){
            cursor = 0; // save
        }
        if(cursor == 9){
            cursor = 8;
        }
        if(cursor>12 && cursor<17){
            cursor = 12; // frequency
        }
        if(cursor>18 && cursor<23){
            cursor = 18; // modulation
        }
        if(cursor == 25){
            cursor = 24;
        }
        lcdPositionXY(cursor%16,cursor/16);
        int timer = 200;
        while(!bLEFT && timer){
            delay_ms(1);
            if(!bRIGHT){
                // user holds both buttons
                cursor = cursor_temp; // save cursor position
                lcdPositionXY(0,0); // set cursor to starting point
                lcdWriteString("**** SAVING ****\n");
                if(flashWriteChanges()){ // save changes to flash
                    lcdWriteString("  Please wait!  ");
                }
                else{
                    lcdWriteString("   No changes   ");
                }
                delay_ms(3000);
                lcdDisplayClear(); // clear up display
                timer = 0;
                menuShow(); // show back menu
                while(!bLEFT || !bRIGHT); // wait for user to release both buttons
            }
            timer--;
        }
        delay_ms(10); // de-bounce
    }
}

void moveCursorUD(void){
    // button UP
    if(!bUP){
        switch(cursor){
        case 0:
            program++;
            if(program>9){
                program = 9;
            }
            menuLoadDataFromMemoryToWorkingRAM(program); // program change, load values from RAM
            break;
        case 5:
            frequency += 1000000;
            break;
        case 6:
            frequency += 100000;
            break;
        case 7:
            frequency += 10000;
            break;
        case 8:
            frequency += 1000;
            break;
        case 10:
            frequency += 100;
            break;
        case 11:
            frequency += 10;
            break;
        case 12:
            frequency += 1;
            break;
        case 17:
            power += 10;
            break;
        case 18:
            power += 1;
            break;
        case 23:
            modulation +=10000;
            break;
        case 24:
            modulation +=1000;
            break;
        case 26:
            modulation +=100;
            break;
        case 27:
            modulation +=10;
            break;
        case 28:
            modulation +=1;
            break;
        }
        if(frequency>7000000){
            frequency = 7000000;
        }

        power = calibrationReturnPower(frequency,power);

        if(modulation>50000){
            modulation = 50000;
        }
        // show changes and wait for a button release
        menuShow();
        usart_send_report();
        while(!bUP);
        delay_ms(10);
    }
    // button DOWN
    if(!bDOWN){
        switch(cursor){
        case 0:
            program--;
            if(program<0){
                program = 0;
            }
            menuLoadDataFromMemoryToWorkingRAM(program); // program change, load values from RAM
            break;
        case 5:
            frequency -= 1000000;
            break;
        case 6:
            frequency -= 100000;
            break;
        case 7:
            frequency -= 10000;
            break;
        case 8:
            frequency -= 1000;
            break;
        case 10:
            frequency -= 100;
            break;
        case 11:
            frequency -= 10;
            break;
        case 12:
            frequency -= 1;
            break;
        case 17:
            power -= 10;
            break;
        case 18:
            power -= 1;
            break;
        case 23:
            modulation -=10000;
            break;
        case 24:
            modulation -=1000;
            break;
        case 26:
            modulation -=100;
            break;
        case 27:
            modulation -=10;
            break;
        case 28:
            modulation -=1;
            break;
        }
        if(frequency<0){
            frequency = 0;
        }

        power = calibrationReturnPower(frequency,power);

        if(modulation<0){
            modulation = 0;
        }
        // show changes and wait for a button release
        menuShow();
        usart_send_report();
        while(!bDOWN);
        delay_ms(10);
    }
}
