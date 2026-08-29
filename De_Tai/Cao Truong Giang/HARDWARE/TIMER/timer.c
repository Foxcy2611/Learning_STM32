#include "timer.h"
#include "led7.h"

volatile uint8_t g_flag_5s_log = 0;
volatile uint8_t g_flag_display_update = 0;

// Khoi tao TIM2 (quet LED 0.5ms) va TIM3 (dem 5s)
void Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);

    // TIM2: 72MHz / (71 + 1) = 1MHz -> ARR = 499 (0.5ms = 2000Hz)
    TIM_TimeBaseStructure.TIM_Period            = 499;
    TIM_TimeBaseStructure.TIM_Prescaler         = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // NVIC cho TIM2
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);

    // TIM3: 72MHz / (7199 + 1) = 10kHz -> ARR = 49999 (5000ms = 5s)
    TIM_TimeBaseStructure.TIM_Period            = 49999;
    TIM_TimeBaseStructure.TIM_Prescaler         = 7199;
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    // NVIC cho TIM3
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM3, ENABLE);
}

// Ngat TIM2 (0.5ms): quet led 7 doan va dem tao chu ky 50ms
void TIM2_Update_ISR_Handler(void)
{
    static uint16_t s_display_count = 0;

    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

        // Quet LED 7 doan
        LED7_Scan_ISR();

        // Dem 100 lan 0.5ms = 50ms thi bat co cap nhat so hien thi
        s_display_count++;
        if (s_display_count >= 100)
        {
            s_display_count = 0;
            g_flag_display_update = 1;
        }
    }
}

// Ngat TIM3 (5s): bat co de ghi log Flash
void TIM3_Update_ISR_Handler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        g_flag_5s_log = 1;
    }
}
