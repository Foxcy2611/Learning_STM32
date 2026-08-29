#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"
#include "PCF8574.h"
#include "LCD.h"
#include "SPI.h"
#include "RC522.h"
#include "stdio.h"
#include "flash.h"
#include "uart.h"
void TIM_config(void)
{
    TIM_TimeBaseInitTypeDef tim;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_Period = 0xFFFF;
    tim.TIM_Prescaler = 71;
    tim.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &tim);
    TIM_Cmd(TIM2, ENABLE);
}
void delay_us(uint16_t cnt)
{
    uint16_t start = TIM_GetCounter(TIM2);
    while ((uint16_t)(TIM_GetCounter(TIM2) - start) < cnt);
}
void delay_ms(uint16_t cnt)
{
    while (cnt--) delay_us(1000);
}

typedef struct {
    uint8_t uid[4];
    char name[16];
} Student;

#define NUM_STUDENTS 3
Student student_list[NUM_STUDENTS] = {
    {{0x03, 0x6C, 0x90, 0x1D}, "Vu Hong Ky"}, // UID th mu 1 (Da dang ky: 036C901D)
    {{0xE3, 0xD1, 0xFA, 0x2E}, "Nguyen Duc Hai"},   // UID thẻ mẫu 2
    {{0x25, 0x36, 0x47, 0x58}, "Phung Thanh Do"}     // UID thẻ mẫu 3
};



int main(void)
{
    uint8_t card_uid[5];
    uint8_t status;

    /* =========================
       Khởi tạo hệ thống
       ========================= */
    TIM_config();
    I2C1_Init();
    LCD_Init();
    SPI1_Init();
    RST_Init();
    UART1_Init();

    // Reset phần cứng RC522 bằng chân RST
    MFRC522_RST_LOW();
    delay_ms(10);
    MFRC522_RST_HIGH();
    delay_ms(50);

    // Khởi tạo thư viện RC522 (Reset mềm và cấu hình thanh ghi nội bộ)
    MFRC522_Init();

    // Khởi tạo Flash: Nếu Flash trống thì lưu danh sách mặc định vào Flash
    uint32_t flash_count = *(volatile uint32_t*)FLASH_UID_PAGE;
    if (flash_count == 0xFFFFFFFF || flash_count == 0)
    {
        uint8_t default_uids[NUM_STUDENTS][4];
        for (int i = 0; i < NUM_STUDENTS; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                default_uids[i][j] = student_list[i].uid[j];
            }
        }
        Flash_SaveUIDs(default_uids, NUM_STUDENTS);
    }

    while (1)
    {
        // Hiển thị màn hình chờ điểm danh mặc định
        LCD_SetCursor(0, 0);
        LCD_Print("HE THONG RFID   ");
        LCD_SetCursor(1, 0);
        LCD_Print("QUET THE...     ");

        // Quét tìm thẻ RFID
        status = MFRC522_Check(card_uid);      
        if (status == MI_OK)
        {
            // Tìm thấy thẻ, bắt đầu so khớp UID bằng cách tra cứu trong Flash
            int found = Flash_FindUID(card_uid);

            if (found != -1)
            {
                // Điểm danh thành công
                LCD_SetCursor(0, 0);
                LCD_Print("DIEM DANH OK!   ");
                LCD_SetCursor(1, 0);
                
                char display_buf[20];
                sprintf(display_buf, "%-16s", student_list[found].name);
                LCD_Print(display_buf);

               
            }
            else
            {
                // Thẻ chưa được đăng ký trong hệ thống
                LCD_SetCursor(0, 0);
                LCD_Print("THE CHUA DK!    ");
                LCD_SetCursor(1, 0);
                
                char display_buf[20];
                sprintf(display_buf, "UID:%02X%02X%02X%02X     ", 
                        card_uid[0], card_uid[1], card_uid[2], card_uid[3]);
                LCD_Print(display_buf);

               
            }

            // Giữ thông báo kết quả trong 2.5 giây
            delay_ms(2500);
        }

        // Chờ 100ms trước lần quét tiếp theo
        delay_ms(100);
    }
}