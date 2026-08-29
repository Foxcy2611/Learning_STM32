#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

// Co bao chu ky 5s ghi log Flash va 50ms cap nhat LED
extern volatile uint8_t g_flag_5s_log;
extern volatile uint8_t g_flag_display_update;

void Timer_Init(void);
void TIM2_Update_ISR_Handler(void);
void TIM3_Update_ISR_Handler(void);

#endif /* __TIMER_H */
