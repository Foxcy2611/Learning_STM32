#ifndef __UART_COMM_H
#define __UART_COMM_H

#include "stm32f10x.h"
#include <string.h>

// Khoi tao UART1 voi baudrate
void UART_Init(uint32_t baudrate);

// Gui chuoi ky tu qua UART
void UART_SendString(char *str);

// Xu ly nhan lenh tu Hercules
uint8_t UART_IsCommandReceived(void);
void UART_GetCommand(char *buf);

// Kiem tra va phan hoi thong tin INFO_REQUEST
void UART_HandleInfoRequest(void);

#endif
