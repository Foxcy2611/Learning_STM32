#include "main.h"

// Tinh do lech tuyet doi
static uint16_t Abs_Diff(uint16_t a, uint16_t b)
{
    return (a > b) ? (a - b) : (b - a);
}

int main(void)
{
    uint16_t current_adc_val = 0;
    uint16_t prev_display_val = 0xFFFF;
    uint16_t sample_num = 0;

    // Khoi tao he thong clock (72MHz)
    SystemInit();

    // Khoi tao LED 7 doan (remap de dung PB3, PB4)
    LED7_Init();

    // Khoi tao ADC1 doc PA0 bang DMA1
    ADC_DMA_Init();

    // Khoi tao Flash doc/ghi log (Page 63)
    FLASH_Logger_Init();

    // Khoi tao UART1 9600 baud
    UART_Info_Init(9600);

    // Khoi tao Timer (TIM2 quet LED, TIM3 dem 5s ghi log)
    Timer_Init();

    // Gui thong tin khoi dong qua UART
    UART_SendString("\r\n========================================================\r\n");
    UART_SendString("    HE THONG DO MUC ANH SANG & GHI LOG FLASH (STM32)    \r\n");
    UART_SendString("========================================================\r\n");
    UART_SendString("[STATUS] He thong da khoi dong thanh cong!\r\n");
    UART_SendString("[FLASH]  So mau log hien tai trong Page 63: ");
    UART_SendNumber(FLASH_Logger_GetSampleCount());
    UART_SendString(" / 50 mau toi da\r\n");
    UART_SendString("[UART]   San sang nhan lenh 'INFO_REQUEST' hoac 'HELP'\r\n");
    UART_SendString("========================================================\r\n\r\n");

    while (1)
    {
        // Cap nhat gia tri hien thi LED 7 doan moi 50ms
        if (g_flag_display_update != 0)
        {
            g_flag_display_update = 0;

            // Doc gia tri ADC da loc trung binh
            current_adc_val = ADC_DMA_GetValue();

            // Loc rung so (deadband khoang 4 don vi)
            if (Abs_Diff(current_adc_val, prev_display_val) >= 4 ||
                current_adc_val == 0 || current_adc_val == 4095 || prev_display_val == 0xFFFF)
            {
                LED7_SetNumber(current_adc_val);
                prev_display_val = current_adc_val;
            }
        }

        // Dinh ky 5s ghi log vao Flash
        if (g_flag_5s_log != 0)
        {
            g_flag_5s_log = 0;

            // Ghi gia tri ADC vao Flash
            if (FLASH_Logger_WriteSample(current_adc_val))
            {
                sample_num = FLASH_Logger_GetSampleCount();
                UART_SendString("[LOG 5s] Mau ");
                UART_SendNumber(sample_num);
                UART_SendString("/50 -> Flash: ADC = ");
                UART_SendNumber(current_adc_val);
                UART_SendString(" (");
                UART_SendNumber(((uint32_t)current_adc_val * 100) / 4095);
                UART_SendString("%)\r\n");
            }
            else
            {
                UART_SendString("[ERROR] Ghi du lieu vao Flash that bai!\r\n");
            }
        }

        // Kiem tra va xu ly lenh UART
        UART_ProcessCommand();
    }
}
