#ifndef __LCD_I2C_H
#define __LCD_I2C_H

#include "stm32f10x.h"
#include "delay.h"

#define LCD_I2C_ADDRESS 0x4E 

void LCD_Init(void);
void LCD_Send_Cmd(char cmd);
void LCD_Send_Data(char data);
void LCD_Send_String(char *str);
void LCD_Goto_XY(uint8_t row, uint8_t col);
void LCD_Clear(void);

#endif