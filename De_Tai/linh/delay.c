#include "delay.h"

void Delay_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; 
    
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF; 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_Cmd(TIM2, ENABLE);
}

/* Hàm delay us */
void Delay_us(uint16_t us) {
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < us);
}

/* Hàm delay ms */
void Delay_ms(uint16_t ms) {
    while (ms--) {
        Delay_us(1000);   //1000 us = 1 ms
    }
}

void Timer_ResetCounter(void) {
    TIM_SetCounter(TIM2, 0);
}

uint16_t Timer_GetCounter(void) {
    return TIM_GetCounter(TIM2);
}