#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f0xx.h"
#include "system_stm32f0xx.h"

#include "pinout.h"
#include "delay.h"
#include "display.h"

void send4bits(unsigned char data){
    E_LOW;
    if(data & 0x1){
        D4_HIGH;
    }
    else{
        D4_LOW;
    }
    if(data & 0x2){
        D5_HIGH;
    }
    else{
        D5_LOW;
    }
    if(data & 0x4){
        D6_HIGH;
    }
    else{
        D6_LOW;
    }
    if(data & 0x8){
        D7_HIGH;
    }
    else{
        D7_LOW;
    }
    E_HIGH;
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    E_LOW;
    __NOP();
    __NOP();
    __NOP();
    __NOP();
}

void lcdSendData(unsigned char c){
    RS_HIGH;
    delay_us(5); // 10
    send4bits(c>>4);
    delay_us(30); // 100
    send4bits(c);
    delay_us(30); // 100
}

void lcdSendCommand(unsigned char c){
    RS_LOW;
    delay_us(10);
    send4bits(c>>4);
    delay_us(100);
    send4bits(c);
    delay_us(100);
}

void lcdInit(void){
    RS_LOW;
    E_LOW;
    D4_LOW;
    D5_LOW;
    D6_LOW;
    D7_LOW;
    delay_ms(30); // power-supply stabilization delay

    send4bits(0x3);
    delay_ms(6);

    send4bits(0x3);
    delay_us(150);

    send4bits(0x3);
    delay_us(100);

    send4bits(0x2);
    delay_us(100);

    lcdSendCommand(0x28); // LCD in 4-bit mode, 2 Line and 5x7 font character
    delay_us(100);
    lcdSendCommand(0x08); // Display off
    delay_us(100);
    lcdSendCommand(0x01); // Clear LCD Screen
    delay_ms(2);
    lcdSendCommand(0x06); // Entry Mode
    delay_us(100);
    lcdSendCommand(0x80); // Start from first line
    delay_us(100);
    lcdSendCommand(0x0E); // Display on cursor on
    delay_us(100);
}

void lcdWriteChar(char character){
    lcdSendData(character);
}

void lcdWriteString(char *str){
    unsigned char i = 0;
    while (str[i] != '\0') {
        // Read characters and increment index
        if(str[i] == '\n'){
            lcdPositionXY(0,1);
            i++;
            continue;
        }
        lcdSendData(str[i]);
        i++;
    }
}

void lcdDisplayClear(void){
    lcdSendCommand(0x01); // Clear LCD Screen
    delay_ms(2);
}

char lcdPositionXY(char x, char y){
    // Check which row
    if (y == 0) {
        // Send instruction 1. row
        lcdSendCommand(0x80 | x);
    } else if (y == 1) {
        // Send instruction 2. row
        lcdSendCommand(0x80 | (0x40 + x));
    }
    delay_us(40);
    return 0;
}
