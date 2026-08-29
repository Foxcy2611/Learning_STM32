#include "uart_comm.h"

#define RX_BUFFER_SIZE 64

static char rx_buffer[RX_BUFFER_SIZE];
static volatile uint8_t rx_index = 0;
static volatile uint8_t cmd_ready = 0;
static char current_command[RX_BUFFER_SIZE];

void UART_Init(uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 1. Cap xung nhip cho USART1, GPIOA va AFIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // 2. Cau hinh TX (PA9) la Alternate Function Push Pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. Cau hinh RX (PA10) la Input Floating
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 4. Cau hinh UART1
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // 5. Cau hinh ngat nhan (RX)
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 6. Cho phep UART1 hoat dong
    USART_Cmd(USART1, ENABLE);
}

void UART_SendString(char *str) {
    while (*str) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, (uint16_t)*str++);
    }
}

uint8_t UART_IsCommandReceived(void) {
    return cmd_ready;
}

void UART_GetCommand(char *buf) {
    strcpy(buf, current_command);
    cmd_ready = 0; 
}

void UART_HandleInfoRequest(void) {
    if (UART_IsCommandReceived()) {
        char cmd[64];
        UART_GetCommand(cmd);
        if (strcmp(cmd, "INFO_REQUEST") == 0) {
            UART_SendString("+ Name: Nhat Linh\r\n");
            UART_SendString("+ Project Name: Canh bao vat can US015\r\n");
            UART_SendString("+ Hardware Used: STM32, US015, LCD1602, CH340\r\n");
            UART_SendString("+ Software Used: keil c\r\n");
            UART_SendString("+ Date: 26/8\r\n");
        }
    }
}

// Trinh phuc vu ngat nhan du lieu UART1
void USART1_IRQHandler(void) {
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET) {
        char c = (char)USART_ReceiveData(USART1);
        
        // Bat ky tu ket thuc lenh (\r hoac \n)
        if (c == '\r' || c == '\n') {
            if (rx_index > 0) {
                rx_buffer[rx_index] = '\0';
                strcpy(current_command, rx_buffer);
                cmd_ready = 1;
                rx_index = 0;
            }
        } else {
            if (rx_index < RX_BUFFER_SIZE - 1) {
                rx_buffer[rx_index++] = c;
            } else {
                rx_index = 0;
            }
            rx_buffer[rx_index] = '\0';
            
            // Phat hien lenh ma khong can enter
            if (strstr(rx_buffer, "INFO_REQUEST") != NULL) {
                strcpy(current_command, "INFO_REQUEST");
                cmd_ready = 1;
                rx_index = 0;
            }
        }
    }
}
