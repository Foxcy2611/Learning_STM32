#include "us015.h"

/*Trigger PA0) và Echo (PA1) */
void US015_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
// trig output pushpull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
// echo output floating
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOA, GPIO_Pin_0); //trig ve 0
}

/* Hàm do do rong xung Echo */
float US015_GetDistance(void) {
    uint16_t time = 0; 
    float distance = 0.0;
    uint32_t timeout = 0;

    // 1. Phát xung kích Trigger 10us
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
    Delay_us(10);
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);

    // 2. cho Echo len 1
    timeout = 0;
    while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == RESET) {
        timeout++;
        if(timeout > 60000) return -1.0; // tranh tran neu dut day cam bien
    }

    Timer_ResetCounter(); 

    timeout = 0;
    while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == SET) {
        timeout++;
        if(timeout > 60000) return -1.0; // Ch?ng treo
    }

    // thoi gian xung Echo
    time = Timer_GetCounter();
		
    distance = (float)time / 58;

    return distance;
}