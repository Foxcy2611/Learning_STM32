#include "alarm.h"

void Alarm_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_13); // Tat ban dau (Active LOW)
}

void Alarm_SetState(uint8_t state) {
    if (state) {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);   // bat (Active LOW)
    } else {
        GPIO_SetBits(GPIOC, GPIO_Pin_13); // tat (Active LOW)
    }
}
