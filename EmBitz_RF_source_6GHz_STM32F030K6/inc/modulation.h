#ifndef MODULATION_H_INCLUDED
#define MODULATION_H_INCLUDED

void timer3_init(void);
void modulationSet(int mod_freq);
void TIM3_IRQHandler(void);

#endif /* MODULATION_H_INCLUDED */
