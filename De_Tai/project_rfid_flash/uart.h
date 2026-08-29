#ifndef __UART_H
#define __UART_H

#include "stm32f10x.h"

/* UART1 */
void UART1_Init(void);

/* Send data */
void UART1_SendChar(char c);
void UART1_SendString(char *str);

/* Send HEX */
void UART1_SendHex(uint8_t data);

/* Send UID */
void UART1_SendUID(uint8_t *uid);

#endif