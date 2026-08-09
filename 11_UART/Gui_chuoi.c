#include "Gui_chuoi.h"

void USART_SendChar(char c){
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, c);
}

void USART_SendString(char *str){
	while(*str){
		USART_SendChar(*str++);
	}
}
