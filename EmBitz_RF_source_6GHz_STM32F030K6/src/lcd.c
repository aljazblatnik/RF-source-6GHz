#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f0xx.h"
#include "system_stm32f0xx.h"

#include "pinout.h"
#include "delay.h"
#include "lcd.h"

void LCD_DisplayClear(void){
    // Diplay clear and return to 0,0
    LCD_SendInstruction(HD44780_DISP_CLEAR);
}

void LCD_DisplayOn(void){
    // send instruction - display on
    LCD_SendInstruction(HD44780_DISP_ON);
}

void LCD_CursorOn(void){
    // send instruction - cursor on
    LCD_SendInstruction(HD44780_CURSOR_ON);
}

void LCD_CursorBlink(void){
    // send instruction - Cursor blink
    LCD_SendInstruction(HD44780_CURSOR_BLINK);
}

void LCD_WriteChar(char character){
    LCD_SendData(character);
}

void LCD_WriteString(char *str){
    unsigned char i = 0;
    while (str[i] != '\0') {
        //read characters and increment index
        LCD_SendData(str[i++]);
    }
}

char LCD_PositionXY(char x, char y){
    if (x > HD44780_COLS || y > HD44780_ROWS) {
        // error
        return ERROR;
    }
    // check which row
    if (y == 0) {
        // send instruction 1st row
        LCD_SendInstruction(HD44780_POSITION | (HD44780_ROW1_START + x));
    } else if (y == 1) {
        // send instruction 2nd row
        LCD_SendInstruction(HD44780_POSITION | (HD44780_ROW2_START + x));
    }
    // success
    return 0;
}

char LCD_Shift(char item, char direction){
    // check if item is cursor or display or direction is left or right
    if ((item != HD44780_DISPLAY) && (item != HD44780_CURSOR)) {
        // error
        return ERROR;
    }
    // check if direction is left or right
    if ((direction != HD44780_RIGHT) && (direction != HD44780_LEFT)) {
        // error
        return ERROR;
    }

    // cursor shift
    if (item == HD44780_CURSOR) {
        // right shift
        if (direction == HD44780_RIGHT) {
            // shit cursor / display to right / left
            LCD_SendInstruction(HD44780_SHIFT | HD44780_CURSOR | HD44780_RIGHT);
        } else {
            // shit cursor / display to right / left
            LCD_SendInstruction(HD44780_SHIFT | HD44780_CURSOR | HD44780_LEFT);
        }
        // display shift
    } else {
        // right shift
        if (direction == HD44780_RIGHT) {
            // shit cursor / display to right / left
            LCD_SendInstruction(HD44780_SHIFT | HD44780_DISPLAY | HD44780_RIGHT);
        } else {
            // shit cursor / display to right / left
            LCD_SendInstruction(HD44780_SHIFT | HD44780_DISPLAY | HD44780_LEFT);
        }
    }
    // success
    return 0;
}

void LCD_Init(void){
    RS_LOW; // clear RS
    E_LOW; // clear E
    delay_ms(16); // delay > 15ms

    LCD_SendInstruction(HD44780_INIT_SEQ); // Initial sequence 0x30 - send 4 bits in 4 bit mode
    delay_ms(5); // delay > 4.1ms

    LCD_PulseE(); // pulse E
    delay_us(110); // delay > 100us

    LCD_PulseE(); // pulse E
    delay_us(50); // delay > 45us (=37+4 * 270/250)

    LCD_Send4bitsIn4bitMode(HD44780_4BIT_MODE); // 4 bit mode 0x20 - send 4 bits in 4 bit mode
    LCD_PulseE(); // pulse E
    delay_us(50); // delay > 45us (=37+4 * 270/250)

    LCD_SendInstruction(HD44780_4BIT_MODE | HD44780_2_ROWS | HD44780_FONT_5x8); // 4-bit & 2-lines & 5x8-dots 0x28
    //LCD_SendInstruction(HD44780_DISP_OFF); // display off 0x08
    LCD_SendInstruction(HD44780_DISP_CLEAR); // display clear 0x01
    LCD_SendInstruction(HD44780_ENTRY_MODE); // entry mode set 0x06
}


void LCD_SendInstruction(unsigned short int data){
    RS_LOW; // Clear RS
    LCD_Send8bitsIn4bitMode(data); // send required data in required mode
    delay_us(10);
}

void LCD_SendData(unsigned short int data){
    RS_HIGH; // Set RS
    LCD_Send8bitsIn4bitMode(data); // send required data in required mode
    delay_us(10);
    RS_LOW; // Clear RS
}

void LCD_Send4bitsIn4bitMode(unsigned short int data){
    E_HIGH; // Set E
    LCD_SetUppNibble(data); // send data to LCD// send data to LCD
    delay_us(1);
    E_LOW;
    delay_us(1);
}

void LCD_Send8bitsIn4bitMode(unsigned short int data)
{
    E_HIGH;
    LCD_SetUppNibble(data); // send data to LCD
    delay_us(1);
    E_LOW;
    delay_us(1);
    // Send lower nibble
    E_HIGH;
    LCD_SetUppNibble(data << 4);
    delay_us(1);
    E_LOW;
    delay_us(1);
}

void LCD_SetUppNibble(unsigned short int data){
    // clear bits DB7-DB4
    D4_LOW;
    D5_LOW;
    D6_LOW;
    D7_LOW;

    // set DB7-DB4 if corresponding bit is set
    if (data & 0x80) { D7_HIGH; }
    if (data & 0x40) { D6_HIGH; }
    if (data & 0x20) { D5_HIGH; }
    if (data & 0x10) { D4_HIGH; }
}

void LCD_PulseE(void){
    E_HIGH; // Set E
    delay_us(1); // PWeh delay time > 450ns
    E_LOW; // Clear E
    delay_us(1); // TcycE > 1000ns -> delay depends on PWeh delay time - delay = TcycE - PWeh = 1000 - 500 = 500ns
}
