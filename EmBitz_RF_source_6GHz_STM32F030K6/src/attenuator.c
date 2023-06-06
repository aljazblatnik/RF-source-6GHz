#include "stm32f0xx.h"
#include "system_stm32f0xx.h"

#include "pinout.h"
#include "delay.h"

void attenuator_send(unsigned char data){
    // send data to attenuator
    aLE_LOW;
    int i;
    for(i = 0; i < 8; i++){
        if(data & 0x1){
            aMOSI_HIGH;
        }
        else{
            aMOSI_LOW;
        }
        aCLK_HIGH;
        data = data>>1;
        delay_us(1);
        aCLK_LOW;
    }
    aLE_HIGH;
    delay_us(1);
    aLE_LOW;
}

void attenuator_set_attenuation(float attenuation){
    unsigned char setting = 0;
    if(attenuation > 31.75){
        setting = 0xFF; // set to a max. attenuation
    }
    else if(attenuation < 0){
        setting = 0x00; // set to a min. attenuation
    }
    else{
        setting = attenuation/0.25;
        // attenuation is rounded down (less than intermittent states)
    }
    attenuator_send(setting);
}

void attenuator_init(void){
    aLE_LOW;
    aCLK_LOW;
    aMOSI_LOW;

    // set maximum attenuation
    attenuator_send(0b11111111);
}

