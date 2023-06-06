#ifndef MAX2871_H_INCLUDED
#define MAX2871_H_INCLUDED

void MAX2871_init(void);
void send_MAX2871(unsigned char addr, unsigned int data);
unsigned int read_MAX2871(void);
int MAX2871_set_frequency(int frequency);

#endif /* MAX2871_H_INCLUDED */
