#include "timer.h"
#include "led7.h"

/* Định nghĩa cờ báo hiệu định thời 5 giây */
volatile uint8_t g_flag_5s_log = 0;

/* Định nghĩa cờ báo hiệu cập nhật màn hình mỗi 50ms (Hướng 2) */
volatile uint8_t g_flag_display_update = 0;

/**
  * @brief  Khởi tạo TIM2 (ngắt 0.5ms = 2000Hz) và TIM3 (ngắt 5 giây) kèm NVIC
  * @param  None
  * @retval None
  */
void Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 1. Cấp xung nhịp cho TIM2 và TIM3 từ APB1 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);

    /* =========================================================================
     * 2. CẤU HÌNH TIM2 (ĐỊNH THỜI 0.5ms = 2000Hz QUÉT LED 7 ĐOẠN SIÊU MƯỢT)
     * Tần số Timer Clock = 72MHz.
     * Prescaler = 71 -> Tần số đếm = 72MHz / (71 + 1) = 1MHz (1 tick = 1us).
     * Period (ARR) = 499 -> Chu kỳ ngắt = 500 tick = 0.5ms (2000Hz).
     * Tần số làm tươi toàn bộ 4 số: 2000Hz / 4 = 500Hz.
     * ========================================================================= */
    TIM_TimeBaseStructure.TIM_Period            = 499;
    TIM_TimeBaseStructure.TIM_Prescaler         = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    /* Xóa cờ và bật ngắt tràn Timer 2 */
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    /* Cấu hình NVIC cho TIM2 (Ưu tiên mức cao nhất 0) */
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Bắt đầu chạy TIM2 */
    TIM_Cmd(TIM2, ENABLE);

    /* =========================================================================
     * 3. CẤU HÌNH TIM3 (ĐỊNH THỜI 5.0 GIÂY KÍCH HOẠT GHI LOG FLASH)
     * Tần số Timer Clock = 72MHz.
     * Prescaler = 7199 -> Tần số đếm = 72MHz / (7199 + 1) = 10kHz (1 tick = 0.1ms).
     * Period (ARR) = 49999 -> Chu kỳ ngắt = 50000 * 0.1ms = 5000ms = 5.0 giây.
     * ========================================================================= */
    TIM_TimeBaseStructure.TIM_Period            = 49999;
    TIM_TimeBaseStructure.TIM_Prescaler         = 7199;
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* Xóa cờ và bật ngắt tràn Timer 3 */
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    /* Cấu hình NVIC cho TIM3 */
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Bắt đầu chạy TIM3 */
    TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  Trình xử lý ngắt tràn TIM2 (0.5ms)
  *         - Quét LED 7 đoạn ở tần số 2000Hz
  *         - Bật cờ g_flag_display_update mỗi 50ms (100 * 0.5ms) để cập nhật số mới
  * @param  None
  * @retval None
  */
void TIM2_Update_ISR_Handler(void)
{
    static uint16_t s_display_count = 0;

    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

        /* 1. Quét hiển thị 1 chữ số LED 7 đoạn */
        LED7_Scan_ISR();

        /* 2. Đếm định thời 50ms để cập nhật giá trị mới cho màn hình */
        s_display_count++;
        if (s_display_count >= 100) /* 100 x 0.5ms = 50ms (20 lần/giây) */
        {
            s_display_count = 0;
            g_flag_display_update = 1;
        }
    }
}

/**
  * @brief  Trình xử lý ngắt tràn TIM3 (5s)
  * @param  None
  * @retval None
  */
void TIM3_Update_ISR_Handler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        /* Bật cờ thông báo chu kỳ 5 giây */
        g_flag_5s_log = 1;
    }
}
