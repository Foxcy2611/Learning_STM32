#include "Nhan_chuoi.h"

/*
Ky tu ket thuc \n

\nLED ON\n
*/


uint8_t USART_Received_String(char *buffer){
	uint8_t idx = 0;
	char rx_data;
	
	while(1){
		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
		rx_data = (char)USART_ReceiveData(USART1);
		
		if(rx_data == '\n' || rx_data == '\r'){ // Da nhan het chuoi
			if(idx > 0){ 
				buffer[idx] = '\0';
				return 1;
			}
		} else {
			buffer[idx++] = rx_data;
		}
	}
}
