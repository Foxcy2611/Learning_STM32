#include "stm32f10x_it.h"
#include "timer.h"
#include "uart_info.h"

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    while (1)
    {
    }
}

void MemManage_Handler(void)
{
    while (1)
    {
    }
}

void BusFault_Handler(void)
{
    while (1)
    {
    }
}

void UsageFault_Handler(void)
{
    while (1)
    {
    }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/******************************************************************************/

/**
  * @brief  Trình phục vụ ngắt ngoại vi TIM2 (Chu kỳ 1ms - Quét LED 7 đoạn)
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
    TIM2_Update_ISR_Handler();
}

/**
  * @brief  Trình phục vụ ngắt ngoại vi TIM3 (Chu kỳ 5 giây - Kích hoạt ghi Log Flash)
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
    TIM3_Update_ISR_Handler();
}

/**
  * @brief  Trình phục vụ ngắt ngoại vi USART1 (Nhận chuỗi lệnh INFO_REQUEST từ PC)
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
    UART1_RX_ISR_Handler();
}
