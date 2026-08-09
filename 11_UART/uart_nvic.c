#include "uart_nvic.h"

char rx_buffer[100];
volatile uint8_t rx_index = 0;
volatile uint8_t rx_ready = 0;

void UART_NVIC_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);
	
	GPIO_InitTypeDef gp;
	
	gp.GPIO_Mode = GPIO_Mode_AF_PP;
	gp.GPIO_Pin = GPIO_Pin_9;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gp);
	
	gp.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gp.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &gp);
	
	USART_InitTypeDef uart;
	uart.USART_BaudRate = 9600;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &uart);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = USART1_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	USART_Cmd(USART1, ENABLE);
}

void USART1_IRQHandler(void){
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
		char rx_data = (char)USART_ReceiveData(USART1);
		
		if(rx_data == '\n' || rx_data == '\r'){
			if(rx_index > 0){
					rx_buffer[rx_index] = '\0';
					rx_ready = 1;
					rx_index = 0;
			}
		} else {
			if(rx_index < 99 && rx_ready == 0){
					rx_buffer[rx_index++] = rx_data;
			}
		}
	}
}

uint8_t UART_Get_String(char *out_str){
	if(rx_ready == 1){
		strcpy(out_str, (char *)rx_buffer);
		rx_ready = 0;
		return 1;
	}
	return 0;
}