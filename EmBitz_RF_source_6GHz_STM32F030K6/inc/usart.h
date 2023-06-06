#ifndef USART_H_INCLUDED
#define USART_H_INCLUDED

#define usart_buffer 3000

void usart_init(void);

void usart_send_string(char *str);
void usart_send_byte(unsigned char byte);
void usart_send_report(void);

int command_decode(char *str);
int compare_string(char *first, char *second);
int string_to_int(char *str, int length);

#endif /* USART_H_INCLUDED */

