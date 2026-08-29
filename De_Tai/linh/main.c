#include "stm32f10x.h"
#include "delay.h"
#include "us015.h"
#include "lcd_i2c.h"
#include "alarm.h"
#include "flash_mem.h"
#include "uart_comm.h"
#include <stdio.h>

float threshold = 0.0; // Bien RAM trung gian

int main(void) {
    float distance = 0;
    char buffer[64];

    // Khoi tao cac ngoai vi
    Delay_Init();
    UART_Init(9600);
    US015_Init();
    LCD_Init();
    Alarm_Init();
    
    // 1. TRUOC WHILE: Kiem tra va nap du lieu tu Flash
    threshold = Flash_ReadThreshold();
    if (threshold < 0) { // Neu Flash trong (Lan dau chay code)
        threshold = 5.0; // Ap nguong mac dinh la 5cm
        Flash_WriteThreshold(threshold); // Luu so 5.0 nay vao vinh vien
    }

    LCD_Goto_XY(0, 0);
    LCD_Send_String("Khoang Cach:");

    while(1) {
        // Kiem tra lenh INFO_REQUEST tu UART
        UART_HandleInfoRequest();
        
        distance = US015_GetDistance();
        
        LCD_Goto_XY(1, 0);
        if(distance >= 0) {
            sprintf(buffer, "%.1f cm    ", distance);
            LCD_Send_String(buffer);
            
            // 2. TRONG WHILE: So sanh bang bien RAM, tuyet doi khong goi Flash_Write o day
            if (distance < threshold) {
                Alarm_SetState(1); 
            } else {
                Alarm_SetState(0); 
            }
        } else {
            LCD_Send_String("Loi cam bien! ");
            Alarm_SetState(0);
        }
        
        Delay_ms(200); 
    }
}
