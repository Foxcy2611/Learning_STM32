#ifndef __UART_NVIC_H
#define __UART_NVIC_H

#include "stm32f10x.h"
#include <string.h>

extern char rx_buffer[100];
extern volatile uint8_t rx_ready;

void UART_NVIC_Init(void);
uint8_t UART_Get_String(char *out_str);

#endif