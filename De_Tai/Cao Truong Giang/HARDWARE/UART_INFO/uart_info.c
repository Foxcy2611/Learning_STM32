#include "uart_info.h"
#include "flash_logger.h"
#include "adc_dma.h"
#include <string.h>
#include <stdio.h>

#define UART_RX_BUFFER_SIZE 128

static char g_rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint8_t g_rx_index = 0;
static volatile uint8_t g_cmd_ready = 0;

// Khoi tao UART1: PA9 (TX), PA10 (RX) va bat ngat RX
void UART_Info_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);

    // TX - PA9
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // RX - PA10
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate            = baudrate;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // NVIC cho USART1
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

// Gui 1 ky tu
void UART_SendChar(char c)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, (uint16_t)c);
}

// Gui chuoi ky tu
void UART_SendString(const char *str)
{
    while (*str)
    {
        UART_SendChar(*str++);
    }
}

// Gui so nguyen dang chuoi
void UART_SendNumber(uint32_t num)
{
    char buf[12];
    snprintf(buf, sizeof(buf), "%lu", (unsigned long)num);
    UART_SendString(buf);
}

// Ngat nhan ky tu UART
void UART1_RX_ISR_Handler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        char rx_char = (char)USART_ReceiveData(USART1);

        if (rx_char == '\r' || rx_char == '\n')
        {
            if (g_rx_index > 0)
            {
                g_rx_buffer[g_rx_index] = '\0';
                g_cmd_ready = 1;
                g_rx_index = 0;
            }
        }
        else
        {
            if (g_rx_index < (UART_RX_BUFFER_SIZE - 1) && !g_cmd_ready)
            {
                g_rx_buffer[g_rx_index++] = rx_char;
                g_rx_buffer[g_rx_index] = '\0';

                // Kiem tra chuoi lenh nhan duoc
                if (strstr(g_rx_buffer, "INFO_REQUEST") != NULL ||
                    strstr(g_rx_buffer, "READ_LOGS") != NULL ||
                    strstr(g_rx_buffer, "CLEAR_LOGS") != NULL ||
                    strstr(g_rx_buffer, "HELP") != NULL)
                {
                    g_cmd_ready = 1;
                    g_rx_index = 0;
                }
            }
        }
    }
}

// Xu ly lenh tu may tinh
void UART_ProcessCommand(void)
{
    char cmd_str[UART_RX_BUFFER_SIZE];

    if (!g_cmd_ready)
    {
        return;
    }

    strncpy(cmd_str, g_rx_buffer, sizeof(cmd_str));
    cmd_str[sizeof(cmd_str) - 1] = '\0';
    g_cmd_ready = 0;
    memset(g_rx_buffer, 0, sizeof(g_rx_buffer));

    // Lenh INFO_REQUEST
    if (strstr(cmd_str, "INFO_REQUEST") != NULL)
    {
        UART_SendString("\r\n========================================================\r\n");
        UART_SendString("             THONG TIN DU AN HE THONG STM32            \r\n");
        UART_SendString("========================================================\r\n");
        UART_SendString("+ Name: Cao Truong Giang\r\n");
        UART_SendString("+ Project Name: De 7 - Canh bao muc anh sang, ghi log (LDR)\r\n");
        UART_SendString("+ Hardware Used: STM32F103C8T6, Bien tro/LDR, LED 7 doan 4 so 5641AS, Module CH340 USB-TTL\r\n");
        UART_SendString("+ Software Used: Keil MDK-ARM v5, STM32F10x Standard Peripheral Library, Hercules Terminal\r\n");
        UART_SendString("========================================================\r\n\r\n");
    }
    // Lenh READ_LOGS
    else if (strstr(cmd_str, "READ_LOGS") != NULL)
    {
        uint16_t count = FLASH_Logger_GetSampleCount();
        uint16_t i;
        uint16_t val;

        UART_SendString("\r\n--- DANH SACH LOG TRONG FLASH (Page 63) ---\r\n");
        UART_SendString("Tong so mau hien co: ");
        UART_SendNumber(count);
        UART_SendString(" / 50 mau toi da\r\n");

        for (i = 0; i < count; i++)
        {
            val = FLASH_Logger_ReadSample(i);
            UART_SendString("[Mau ");
            UART_SendNumber(i + 1);
            UART_SendString("] ADC = ");
            UART_SendNumber(val);
            UART_SendString(" (");
            UART_SendNumber(((uint32_t)val * 100) / 4095);
            UART_SendString("%)\r\n");
        }
        UART_SendString("-------------------------------------------\r\n\r\n");
    }
    // Lenh CLEAR_LOGS
    else if (strstr(cmd_str, "CLEAR_LOGS") != NULL)
    {
        FLASH_Logger_ClearAll();
        UART_SendString("\r\n[FLASH] Da xoa trang toan bo vung nho Log Flash Page 63!\r\n\r\n");
    }
    // Lenh HELP
    else if (strstr(cmd_str, "HELP") != NULL)
    {
        UART_SendString("\r\n--- DANH SACH LENH HE THONG ---\r\n");
        UART_SendString("1. INFO_REQUEST : Yeu cau thong tin sinh vien & du an\r\n");
        UART_SendString("2. READ_LOGS    : Doc toan bo mau log trong Flash\r\n");
        UART_SendString("3. CLEAR_LOGS   : Xoa trang toan bo bo nho log Flash\r\n");
        UART_SendString("4. HELP         : Xem huong dan lenh\r\n");
        UART_SendString("-------------------------------\r\n\r\n");
    }
}
