#ifndef LCD_H
#define LCD_H

#include "stm32f10x.h"
#include "PCF8574.h"

#define LCD_EN  2
#define LCD_RW  1
#define LCD_RS  0
#define LCD_D4  4
#define LCD_D5  5
#define LCD_D6  6
#define LCD_D7  7
#define LCD_BL  3

/* LCD Instructions */
#define LCD_CLEAR_DISPLAY      0x01						//Xoa toan bo noi dung hien thi, dua cursor ve dau
#define LCD_RETURN_HOME        0x02						//Dua cursor ve vi tri dau, khong xoa noi dung

#define LCD_ENTRY_MODE_SET     0x04						//Cau hinh cach cursor di chuyen sau khi ghi ky tu
#define LCD_DISPLAY_CONTROL    0x08						//Cau hinh bat/tat display, cursor va blink
#define LCD_CURSOR_SHIFT       0x10						//Dich cursor hoac dich toan bo display
#define LCD_FUNCTION_SET       0x20						//Cau hinh che do giao tiep, so dong va font

#define LCD_SET_CGRAM_ADDR     0x40						//Dat dia chi CGRAM de tao ky tu tuy chinh
#define LCD_SET_DDRAM_ADDR     0x80						//Dat dia chi DDRAM de chon vi tri hien thi


/* Entry Mode */
#define LCD_ENTRY_INCREMENT    0x02						//Cursor tang dia chi sau moi ky tu
#define LCD_ENTRY_DECREMENT    0x00						//Cursor giam dia chi sau moi ky tu
#define LCD_ENTRY_SHIFT_ON     0x01						//Tu dong dich display sau khi ghi ky tu
#define LCD_ENTRY_SHIFT_OFF    0x00						//Khong dich display sau khi ghi ky tu
	

/* Display Control */
#define LCD_DISPLAY_ON         0x04						//Bat hien thi
#define LCD_DISPLAY_OFF        0x00						
#define LCD_CURSOR_ON          0x02						//Hien cursor
#define LCD_CURSOR_OFF         0x00
#define LCD_BLINK_ON           0x01						//Bat nhap nhay cursor
#define LCD_BLINK_OFF          0x00


/* Cursor / Display Shift */
#define LCD_SHIFT_DISPLAY      0x08						//Dich toan bo noi dung display
#define LCD_SHIFT_CURSOR       0x00						//Chi dich cursor
#define LCD_SHIFT_RIGHT        0x04						//Dich sang phai
#define LCD_SHIFT_LEFT         0x00						//Dich sang trai


/* Function Set */
#define LCD_FUNCTION_8BIT      0x10						//Giao tiep LCD 8-bit
#define LCD_FUNCTION_4BIT      0x00						//Giao tiep LCD 4-bit
#define LCD_FUNCTION_2LINE     0x08						//Su dung LCD 2 dong
#define LCD_FUNCTION_1LINE     0x00						//Su dung LCD 1 dong
#define LCD_FUNCTION_5x10      0x04						//Font ky tu 5x10
#define LCD_FUNCTION_5x8       0x00						//Font ky tu 5x8

void LCD_send4bit(uint8_t data);
void LCD_Init(void);
void LCD_SendCommand(uint8_t cmd);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Print(const char *str);

#endif