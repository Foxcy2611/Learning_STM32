#include "uart.h"


/* ================= UART1 INIT ================= */

void UART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* Enable clock GPIOA + USART1 */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_USART1,
        ENABLE
    );

    /* PA9 -> USART1_TX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* PA10 -> USART1_RX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* USART1 configuration */
    USART_InitStructure.USART_BaudRate = 115200;

    USART_InitStructure.USART_WordLength =
        USART_WordLength_8b;

    USART_InitStructure.USART_StopBits =
        USART_StopBits_1;

    USART_InitStructure.USART_Parity =
        USART_Parity_No;

    USART_InitStructure.USART_Mode =
        USART_Mode_Tx | USART_Mode_Rx;

    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;

    USART_Init(USART1, &USART_InitStructure);


    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE);
}


/* ================= SEND CHAR ================= */

void UART1_SendChar(char c)
{
    USART_SendData(USART1, (uint16_t)c);

    while (USART_GetFlagStatus(
               USART1,
               USART_FLAG_TXE
           ) == RESET);
}


/* ================= SEND STRING ================= */

void UART1_SendString(char *str)
{
    while (*str)
    {
        UART1_SendChar(*str);
        str++;
    }
}


/* ================= SEND HEX ================= */

void UART1_SendHex(uint8_t data)
{
    const char hex[] = "0123456789ABCDEF";

    UART1_SendChar(hex[(data >> 4) & 0x0F]);
    UART1_SendChar(hex[data & 0x0F]);
}


/* ================= SEND UID ================= */

void UART1_SendUID(uint8_t *uid)
{
    uint8_t i;

    for (i = 0; i < 4; i++)
    {
        UART1_SendHex(uid[i]);

        if (i < 3)
        {
            UART1_SendChar(' ');
        }
    }
}