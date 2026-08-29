#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

/* Cờ báo hiệu chu kỳ 5 giây để ghi Log vào Flash */
extern volatile uint8_t g_flag_5s_log;

/* Cờ báo hiệu chu kỳ 50ms để cập nhật số mới lên LED 7 đoạn (Hướng 2) */
extern volatile uint8_t g_flag_display_update;

/* Các hàm nguyên mẫu */
void Timer_Init(void);
void TIM2_Update_ISR_Handler(void);
void TIM3_Update_ISR_Handler(void);

#endif /* __TIMER_H */
