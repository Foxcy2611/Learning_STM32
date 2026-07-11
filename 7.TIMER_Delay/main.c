#include "stm32f10x.h"                  // Device header
#include "stm32f10x_tim.h"              // Device:StdPeriph Drivers:TIM

#include "Delay_Timer.h"

static volatile uint32_t msTick = 0;

void TIM3_Config(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseInitTypeDef TM;
	
	TM.TIM_ClockDivision = TIM_CKD_DIV1; // Const
	TM.TIM_CounterMode = TIM_CounterMode_Up; // Const
	TM.TIM_Period = 999; // 1 lan tran = 1ms
	TM.TIM_Prescaler = 71;
	TM.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TM);
	
	// Khai bao ngat cho TIM3 
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC_InitTypeDef NV;
	NV.NVIC_IRQChannelPreemptionPriority = 0;
	NV.NVIC_IRQChannelSubPriority = 0;
	NV.NVIC_IRQChannel = TIM3_IRQn;
	NV.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NV);
	
	// Bat dau hoat dong
	TIM_Cmd(TIM2, ENABLE);
}

void TIM3_IRQHandler(){
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
		msTick++;
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

uint32_t millis(){
	return msTick;
}

void Delay_mss(uint32_t ms){
	uint32_t st_time = millis();
	
	while((millis() - st_time) < ms);
}

void GPIO_Config(void){
	// Blink led PC13
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef gp;
	
	gp.GPIO_Mode = GPIO_Mode_Out_PP;
	gp.GPIO_Pin = GPIO_Pin_13;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gp);
}

void Blink(){
	// ON LED - 0
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	Delay_ms(1000);
	// OFF LED
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	Delay_ms(1000);
}

int main(){
	SystemInit();
	TIM_Init();
	GPIO_Config();
	// TIM3 Config
	while(1){
		Blink();
	}
}
