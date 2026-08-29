#ifndef __UART_INFO_H
#define __UART_INFO_H

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Khởi tạo UART1 và cấu hình ngắt NVIC */
void UART_Info_Init(uint32_t baudrate);

/* Gửi 1 ký tự qua UART1 */
void UART_SendChar(char c);

/* Gửi chuỗi ký tự qua UART1 */
void UART_SendString(const char *str);

/* Gửi số nguyên dạng chuỗi qua UART1 */
void UART_SendNumber(uint32_t num);

/* Hàm xử lý lệnh nhận được từ máy tính (gọi trong vòng lặp chính main) */
void UART_ProcessCommand(void);

/* Trình phục vụ ngắt nhận dữ liệu UART1 */
void UART1_RX_ISR_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __UART_INFO_H */
