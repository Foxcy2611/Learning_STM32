#include "Delay_Timer.h"

/*
TIMER 16-Bit
APB1: 36MHz
APB2: 72MHz

Tat ca TIM deu dung 72MHz

Tao 1 ham delay chuan tung ms: 1s = 1000ms
Tim2
*/

/*
Period: ARR
Prescaler: PSC

+ PSC: Chia nho tan so goc
Tan so goc: 72MHZ = 72 . 10^6
PSC = 71
=> 1 lan dem cua bo timer, no se ton
(PSC + 1) / 72Mhz = 72/(72.10^6) = 1us

+ ARR: Gia tri toi da cua bo dem chinh
Timer dem tu 0, 1, 2, ...., ARR
ARR = 999 => No se phai dem 1000 lan
Ma 1 lan no ton 1us
=> 1000 lan se ton = 1000 . 1us = 1ms


- PSC: 7199
- ARR: 2.10^4 - 1
PSC: 7100
=> 1 tick = (7199 + 1) / 72MHz = 0.0001s = 0.1m = 0.1 . 10^(-3)
ARR: 2.10^4 - 1
No phai dem 20000 lan thi moi tran
=> TONG time: 20000. 0.0001 = 2s
*/

void TIM_Init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitTypeDef TM;
	
	TM.TIM_ClockDivision = TIM_CKD_DIV1; // Const
	TM.TIM_CounterMode = TIM_CounterMode_Up; // Const
	TM.TIM_Period = 0xFFFF;
	TM.TIM_Prescaler = 71;
	TM.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM2, &TM);
	
	// Bat dau hoat dong
	TIM_Cmd(TIM2, ENABLE);
}

void Delay_us(uint16_t us){
	TIM_SetCounter(TIM2, 0);
	while(TIM_GetCounter(TIM2) < us);
}

// 1ms  = 1000 us
void Delay_ms(uint16_t ms){
	while(ms--){
		Delay_us(1000);
	}
}
	