#include "lcd_i2c.h"

void I2C_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Cau hinh I2C
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; // ti le do rong xung (mac dinh la 2)
    I2C_InitStructure.I2C_OwnAddress1 = 0; //dia chi cua STM32 (master thì cu de 0)
    I2C_InitStructure.I2C_Ack = I2C_Ack_Disable; // tat tu dong gui tin hieu xac nhan ACK
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //7 bit
    I2C_InitStructure.I2C_ClockSpeed = 100000; // toc do truyen
    I2C_Init(I2C1, &I2C_InitStructure);
    
    I2C_Cmd(I2C1, ENABLE);
}

/* Hàm truyen 8 bit qua I2C */
void I2C_Write(uint8_t data) {
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, LCD_I2C_ADDRESS, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(I2C1, data);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C1, ENABLE);
}

/* Các hàm giao tiep voi LCD1602 qua chip PCF8574 */
void LCD_Send_Cmd(char cmd) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd & 0xf0);
    data_l = ((cmd << 4) & 0xf0);
    data_t[0] = data_u | 0x0C;  // EN=1, RS=0
    data_t[1] = data_u | 0x08;  // EN=0, RS=0
    data_t[2] = data_l | 0x0C;  // EN=1, RS=0
    data_t[3] = data_l | 0x08;  // EN=0, RS=0
    for(int i = 0; i < 4; i++) I2C_Write(data_t[i]);
}

void LCD_Send_Data(char data) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (data & 0xf0);
    data_l = ((data << 4) & 0xf0);
    data_t[0] = data_u | 0x0D;  // EN=1, RS=1
    data_t[1] = data_u | 0x09;  // EN=0, RS=1
    data_t[2] = data_l | 0x0D;  // EN=1, RS=1
    data_t[3] = data_l | 0x09;  // EN=0, RS=1
    for(int i = 0; i < 4; i++) I2C_Write(data_t[i]);
}

void LCD_Init(void) {
    I2C_Config();
    Delay_ms(50); 
    LCD_Send_Cmd(0x33);
    LCD_Send_Cmd(0x32);
    LCD_Send_Cmd(0x28); 
    LCD_Send_Cmd(0x0C); // B?t màn hình, t?t con tr?
    LCD_Send_Cmd(0x06); // T? d?ng tang con tr?
    LCD_Clear();
}

void LCD_Clear(void) {
    LCD_Send_Cmd(0x01);
    Delay_ms(2);
}

void LCD_Goto_XY(uint8_t row, uint8_t col) {
    uint8_t address;
    if (row == 0) address = 0x80;
    else address = 0xC0;
    LCD_Send_Cmd(address | col);
}

void LCD_Send_String(char *str) {
    while (*str) LCD_Send_Data(*str++);
}