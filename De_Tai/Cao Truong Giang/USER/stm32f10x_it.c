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

// Ngat Timer 2: Quet LED 7 doan
void TIM2_IRQHandler(void)
{
    TIM2_Update_ISR_Handler();
}

// Ngat Timer 3: Dinh ky 5s ghi log Flash
void TIM3_IRQHandler(void)
{
    TIM3_Update_ISR_Handler();
}

// Ngat nhan UART1
void USART1_IRQHandler(void)
{
    UART1_RX_ISR_Handler();
}
