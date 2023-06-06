#ifndef PCD8544_H
#define PCD8544_H

// LCD defines
#define BLACK 1
#define WHITE 0

#define LCDWIDTH 84
#define LCDHEIGHT 48

#define PCD8544_POWERDOWN 0x04
#define PCD8544_ENTRYMODE 0x02
#define PCD8544_EXTENDEDINSTRUCTION 0x01

#define PCD8544_DISPLAYBLANK 0x0
#define PCD8544_DISPLAYNORMAL 0x4
#define PCD8544_DISPLAYALLON 0x1
#define PCD8544_DISPLAYINVERTED 0x5

// H = 0
#define PCD8544_FUNCTIONSET 0x20
#define PCD8544_DISPLAYCONTROL 0x08
#define PCD8544_SETYADDR 0x40
#define PCD8544_SETXADDR 0x80

// H = 1
#define PCD8544_SETTEMP 0x04
#define PCD8544_SETBIAS 0x10
#define PCD8544_SETVOP 0x80


void start(uint8_t contrast, uint8_t bias);

void command(uint8_t c);
void data(uint8_t c);

void setContrast(uint8_t val);
void clearDisplay(void);
void display();

void drawPixel(int16_t x, int16_t y, uint16_t color);
uint8_t getPixel(int8_t x, int8_t y);

void spiWrite(uint8_t c);

#endif
