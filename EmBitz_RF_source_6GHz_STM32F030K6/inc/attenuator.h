#ifndef ATTENUATOR_H_INCLUDED
#define ATTENUATOR_H_INCLUDED

void attenuator_init(void);
void attenuator_send(unsigned char data); // raw data
void attenuator_set_attenuation(float attenuation); // 0 - 31.75 dB

#endif /* ATTENUATOR_H_INCLUDED */
