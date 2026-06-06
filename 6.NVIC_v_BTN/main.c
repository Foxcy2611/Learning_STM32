#include "stm32f10x.h"                  // Device header

// Button PA0
// GPIOA ; GPIO_Pin_0
// Khi nhan nut thi den PC13 sang/tat

// CHinh GPIO
/*
 Cap xung Clock
 Cau hinh chan
 Chot du lieu
*/

uint8_t logic_btn = 0;

void GPIO_Config(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	
	// Button
	GPIO_InitTypeDef gp;
	gp.GPIO_Pin = GPIO_Pin_0;
	gp.GPIO_Mode = GPIO_Mode_IPU; // Mac dinh 1 -> 0
	GPIO_Init(GPIOA, &gp);
	
	// Led PC13
	gp.GPIO_Pin = GPIO_Pin_13;
	gp.GPIO_Mode = GPIO_Mode_Out_PP;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gp);
}

void Delay(uint32_t ms){
	for(uint32_t i = 0 ; i < ms ; i++){
		for(uint32_t j = 0 ; j < 0x2aff ; j++);
	}
}

//////////////////////////////////////////////////
void Read_BTN(void){
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == RESET){
		Delay(1);
		
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == RESET){
			logic_btn = !logic_btn;
		
			if(logic_btn == 0){
				GPIO_ResetBits(GPIOC, GPIO_Pin_13);
			} else {
				GPIO_SetBits(GPIOC, GPIO_Pin_13);
			}
		}
	}
}
//////////////////////////////////////////////

void Cau_hinh_ngat(void){
	EXTI_InitTypeDef ex;
	NVIC_InitTypeDef nv;
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); // PA0
	ex.EXTI_Line = EXTI_Line0;
	ex.EXTI_LineCmd = ENABLE;
	ex.EXTI_Mode = EXTI_Mode_Interrupt;
	ex.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&ex);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	nv.NVIC_IRQChannel = EXTI0_IRQn;
	nv.NVIC_IRQChannelCmd = ENABLE;
	nv.NVIC_IRQChannelPreemptionPriority = 0;
	nv.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nv);
}

void EXTI0_IRQHandler(){
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
		EXTI_ClearITPendingBit(EXTI_Line0);

		logic_btn = !logic_btn;
		
		if(logic_btn == 0){
			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		} else {
			GPIO_SetBits(GPIOC, GPIO_Pin_13);
		}
	}		
}

int main(){
	GPIO_Config();
	Cau_hinh_ngat();
	
	while(1){
		
	}
}
