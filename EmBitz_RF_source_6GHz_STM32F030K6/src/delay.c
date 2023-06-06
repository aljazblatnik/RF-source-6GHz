#include "stm32f0xx.h"
#include "system_stm32f0xx.h"
#include <stdio.h>
#include <stdlib.h>

// tested/optimized with GCC -02 setting

void timer14_init(void){
    // delay timer 1 ms
    RCC->APB1ENR |= (RCC_APB1ENR_PWREN | RCC_APB1ENR_TIM14EN); // enable clock
    //TIM14->CR1 |= TIM_CR1_URS;
    TIM14->PSC = 47999;
    TIM14->ARR = 0;
    TIM14->EGR = TIM_EGR_UG; // Re-initializes the timer counter
}

void delay_ms(int ms){
    TIM14->EGR = TIM_EGR_UG; // Re-initializes the timer counter
    TIM14->ARR = ms;
    TIM14->CNT = 1;
    TIM14->CR1 |= TIM_CR1_CEN; //start counter
    TIM14->SR &= ~TIM_SR_UIF; // Clear flag
    while(!(TIM14->SR & TIM_SR_UIF));
    TIM14->SR &= ~TIM_SR_UIF; // Clear flag
    TIM14->CR1 &= ~TIM_CR1_CEN; //stop counter
}

void timer16_init(void){
    RCC->APB2ENR |= RCC_APB2ENR_TIM16EN; // clock enable
    TIM16->PSC = 47;
    TIM16->ARR = 1;
    TIM16->EGR = TIM_EGR_UG; // Re-initializes the timer counter
}

void delay_us(int us){
    // magic ... don´t touch!
    if(us < 2){
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        if(us == 1){
            return;
        }
    }
    TIM16->EGR = TIM_EGR_UG; // Re-initializes the timer counter
    TIM16->ARR = us;
    TIM16->CNT = 2;
    TIM16->CR1 |= TIM_CR1_CEN; //start counter
    //TIM16->ARR = us;
    TIM16->SR &= ~TIM_SR_UIF; // Clear flag
    //TIM16->CR1 |= TIM_CR1_CEN; //start counter
    while(!(TIM16->SR & TIM_SR_UIF));
    TIM16->CR1 &= ~TIM_CR1_CEN; //stop counter
}
