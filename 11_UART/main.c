#include "stm32f10x.h"                  // Device header
#include "stm32f10x_usart.h"            // Device:StdPeriph Drivers:USART

#include "Gui_chuoi.h"
#include "Nhan_chuoi.h"

#include "uart_nvic.h"

// 

char rx_str[500];

// buffer ngat
char my_str[100];


void GPIO_Config(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitTypeDef gp;
	
	// PA9 - TX - AF PP
	gp.GPIO_Mode = GPIO_Mode_AF_PP;
	gp.GPIO_Pin = GPIO_Pin_9;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gp);
	
	// PA10 - RX - IN FLOATING
	gp.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gp.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &gp);
}
// ab => 2 frame
void USART_Config_not_NVIC(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitTypeDef uart;
	
	uart.USART_BaudRate = 9600;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &uart);
	
	USART_Cmd(USART1, ENABLE);
}

void Delay(uint32_t ms){
	for(uint32_t i = 0 ; i < ms ; i++){
		for(uint32_t j = 0 ; j < 0x2aff ; j++);
	}
}

/*
int main(){
	SystemInit();
	GPIO_Config();
	USART_Config_not_NVIC();

	while(1){
		if(USART_Received_String(rx_str) == 1){
			// Turn On
			USART_SendString("Nhan thanh cong\n");
			Delay(1000);
		}
	}
	
}
*/


int main(void){
	UART_NVIC_Init();
    
	while(1){
		if(UART_Get_String(my_str) == 1){
			USART_SendString("Da nhan qua NVIC: ");
			USART_SendString(my_str);
			USART_SendChar('\n');
		}
			///
	}
}
