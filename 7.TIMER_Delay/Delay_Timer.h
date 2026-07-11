#ifndef DELAY_TIMER_H
#define DELAY_TIMER_H

#include "stm32f10x.h"                  // Device header

void TIM_Init(void);
void Delay_us(uint16_t us);
void Delay_ms(uint16_t ms);


#endif
