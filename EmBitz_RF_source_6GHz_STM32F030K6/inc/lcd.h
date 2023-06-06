#ifndef LCD_H_INCLUDED
#define LCD_H_INCLUDED

// Success
#ifndef SUCCESS
    #define SUCCESS 0
#endif

// Error
#ifndef ERROR
    #define ERROR   1
#endif

#define HD44780_BUSY_FLAG   HD44780_DB7
#define HD44780_INIT_SEQ    0x30
#define HD44780_DISP_CLEAR  0x01
#define HD44780_DISP_OFF    0x08
#define HD44780_DISP_ON 0x0C
#define HD44780_CURSOR_ON   0x0E
#define HD44780_CURSOR_BLINK    0x0F
#define HD44780_RETURN_HOME 0x02
#define HD44780_ENTRY_MODE  0x06
#define HD44780_4BIT_MODE   0x20
#define HD44780_8BIT_MODE   0x30
#define HD44780_2_ROWS  0x08
#define HD44780_FONT_5x8    0x00
#define HD44780_FONT_5x10   0x04
#define HD44780_POSITION    0x80

#define HD44780_SHIFT   0x10
#define HD44780_CURSOR  0x00
#define HD44780_DISPLAY 0x08
#define HD44780_LEFT    0x00
#define HD44780_RIGHT   0x04

#define HD44780_ROWS    2
#define HD44780_COLS    16

#define HD44780_ROW1_START  0x00
#define HD44780_ROW1_END    HD44780_COLS
#define HD44780_ROW2_START  0x40
#define HD44780_ROW2_END    HD44780_COLS

void LCD_Init(void);
void LCD_DisplayClear(void);
void LCD_DisplayOn(void);
void LCD_CursorOn(void);
void LCD_CursorBlink(void);
void LCD_WriteChar(char character);
void LCD_WriteString(char *str);
char LCD_PositionXY(char x, char y);
char LCD_Shift(char item, char direction);
void LCD_SendInstruction(unsigned short int);
void LCD_SendData(unsigned short int);
void LCD_Send4bitsIn4bitMode(unsigned short int);
void LCD_Send8bitsIn4bitMode(unsigned short int);
void LCD_SetUppNibble(unsigned short int);
void LCD_SetLowNibble(unsigned short int);
void LCD_PulseE(void);

#endif /* LCD_H_INCLUDED */
