#ifndef __UART_INFO_H
#define __UART_INFO_H

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void UART_Info_Init(uint32_t baudrate);
void UART_SendChar(char c);
void UART_SendString(const char *str);
void UART_SendNumber(uint32_t num);
void UART_ProcessCommand(void);
void UART1_RX_ISR_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __UART_INFO_H */
