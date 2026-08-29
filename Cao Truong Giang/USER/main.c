#include "main.h"

/**
  * @brief  Hàm tính giá trị tuyệt đối cho số nguyên 16-bit
  */
static uint16_t Abs_Diff(uint16_t a, uint16_t b)
{
    return (a > b) ? (a - b) : (b - a);
}

/**
  * @brief  Hàm chính điều phối toàn bộ hoạt động hệ thống
  * @param  None
  * @retval int
  */
int main(void)
{
    uint16_t current_adc_val = 0;
    uint16_t prev_display_val = 0xFFFF;
    uint16_t sample_num = 0;

    /* 1. Khởi tạo xung nhịp hệ thống (72MHz PLL từ HSE 8MHz) */
    SystemInit();

    /* 2. Khởi tạo Module hiển thị LED 7 đoạn & Remap JTAG (giải phóng PB3, PB4) */
    LED7_Init();

    /* 3. Khởi tạo Module đo ADC1 (PA0) và DMA1 Channel 1 */
    ADC_DMA_Init();

    /* 4. Khởi tạo Module quản lý lưu trữ Flash nội (Page 63) */
    FLASH_Logger_Init();

    /* 5. Khởi tạo Module giao tiếp UART1 (PA9-TX, PA10-RX) với Baudrate 9600 */
    UART_Info_Init(9600);

    /* 6. Khởi tạo Bộ định thời Timer (TIM2: 0.5ms quét LED + 50ms display, TIM3: 5s ghi Log) */
    Timer_Init();

    /* Gửi bản tin chào mừng khởi động hệ thống qua UART */
    UART_SendString("\r\n========================================================\r\n");
    UART_SendString("    HE THONG DO MUC ANH SANG & GHI LOG FLASH (STM32)    \r\n");
    UART_SendString("========================================================\r\n");
    UART_SendString("[STATUS] He thong da khoi dong thanh cong!\r\n");
    UART_SendString("[FLASH]  So mau log hien tai trong Page 63: ");
    UART_SendNumber(FLASH_Logger_GetSampleCount());
    UART_SendString(" / 50 mau toi da\r\n");
    UART_SendString("[UART]   San sang nhan lenh 'INFO_REQUEST' hoac 'HELP'\r\n");
    UART_SendString("========================================================\r\n\r\n");

    /* Vòng lặp điều phối chính (Event Loop) */
    while (1)
    {
        /* --- NHIỆM VỤ 1: CẬP NHẬT HIỂN THỊ LED 7 ĐOẠN ĐỊNH KỲ MỖI 50ms (HƯỚNG 2) --- */
        if (g_flag_display_update != 0)
        {
            /* Xóa cờ cập nhật màn hình */
            g_flag_display_update = 0;

            /* Đọc giá trị ADC đã qua bộ lọc trung bình 16 mẫu */
            current_adc_val = ADC_DMA_GetValue();

            /* Áp dụng ngưỡng lọc biến động (Deadband >= 4 đơn vị) */
            if (Abs_Diff(current_adc_val, prev_display_val) >= 4 ||
                current_adc_val == 0 || current_adc_val == 4095 || prev_display_val == 0xFFFF)
            {
                LED7_SetNumber(current_adc_val);
                prev_display_val = current_adc_val;
            }
        }

        /* --- NHIỆM VỤ 2: KIỂM TRA ĐỊNH THỜI 5 GIÂY ĐỂ GHI LOG FLASH --- */
        if (g_flag_5s_log != 0)
        {
            /* Xóa cờ định thời */
            g_flag_5s_log = 0;

            /* Ghi mẫu ADC vào Flash Page 63 theo bộ đệm vòng */
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

        /* --- NHIỆM VỤ 3: XỬ LÝ CÁC LỆNH NHẬN ĐƯỢC TỪ UART --- */
        UART_ProcessCommand();
    }
}
