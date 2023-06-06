#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

void send4bits(unsigned char data);
void lcdSendData(unsigned char c);
void lcdSendCommand(unsigned char c);
void lcdInit(void);
void lcdWriteChar(char character);
void lcdWriteString(char *str);
void lcdDisplayClear(void);
char lcdPositionXY(char x, char y);

#endif /* DISPLAY_H_INCLUDED */
