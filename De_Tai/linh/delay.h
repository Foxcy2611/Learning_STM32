#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

void Delay_Init(void);

void Delay_us(uint16_t us);
void Delay_ms(uint16_t ms);

// hàm do kc cua US015  s = v.t
void Timer_ResetCounter(void);
uint16_t Timer_GetCounter(void);

#endif 