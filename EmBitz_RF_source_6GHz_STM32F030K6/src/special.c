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

extern char mode;

int step = 0;
int set_frequency = 1563170;

void doSpecialMode(char mode){
    switch(mode){
    case 0:
            if(step != 1){
                MAX2871_set_frequency(1575420);
                step = 1;
            }
            break;
    case 1:
            if(step == 0){
                MAX2871_set_frequency(1575420);
                step = 1;
                delay_ms(1);
            }
            else{
                MAX2871_set_frequency(1601990);
                step = 0;
                delay_ms(1);
            }
            break;
    case 2:
            if(step == 0){
                set_frequency = 1563170;
            }
            else if(step < 245){
                set_frequency += 100;
                step++;
            }
            else{
                set_frequency += 100;
                step = 0;
            }
            MAX2871_set_frequency(set_frequency);
            delay_ms(1);
            break;
    case 3:
            if(step == 0){
                set_frequency = 1563170;
            }
            else if(step < 245){
                set_frequency += 200;
                step++;
            }
            else{
                set_frequency += 200;
                step = 0;
            }
            MAX2871_set_frequency(set_frequency);
            delay_ms(1);
            break;
    default:
            if(step != 1){
                MAX2871_set_frequency(1575420);
                step = 1;
            }
            break;
    }
}
