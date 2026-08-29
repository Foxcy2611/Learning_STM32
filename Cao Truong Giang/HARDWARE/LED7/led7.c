#include "led7.h"

/* Bảng mã 7 đoạn LED Âm chung (Common Cathode): Bit 0 = A, Bit 1 = B, ..., Bit 6 = G, Bit 7 = DP */
static const uint8_t LED7_HEX_CODE[10] = {
    0x3F, /* 0: A B C D E F    */
    0x06, /* 1:   B C          */
    0x5B, /* 2: A B   D E   G  */
    0x4F, /* 3: A B C D     G  */
    0x66, /* 4:   B C   F   G  */
    0x6D, /* 5: A   C D F   G  */
    0x7D, /* 6: A   C D E F G  */
    0x07, /* 7: A B C          */
    0x7F, /* 8: A B C D E F G  */
    0x6F  /* 9: A B C D   F G  */
};

/* Mảng lưu 4 chữ số hiển thị: [0] = Hàng nghìn, [1] = Hàng trăm, [2] = Hàng chục, [3] = Hàng đơn vị */
static volatile uint8_t g_led_digits[4] = {0, 0, 0, 0};

/* Biến chỉ số quét hiện tại (0 -> 3) */
static volatile uint8_t g_scan_index = 0;

/* Trạng thái bật/tắt hiển thị */
static volatile uint8_t g_display_enable = 1;

/**
  * @brief  Khởi tạo các chân GPIO điều khiển LED 7 đoạn và cấu hình Remap giải phóng PB3, PB4
  *         - Thanh đoạn (Anode): PB0, PB1, PB8 (Đoạn C), PB3..PB7 (Mức 1 sáng).
  *         - Digit (Cathode trực tiếp): PB12, PB13, PB14, PB15 (Mức 0 kéo mass sáng / Active LOW).
  * @param  None
  * @retval None
  */
void LED7_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 1. Cấp xung nhịp cho GPIOB và khối chức năng AFIO */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    /* 2. Thực hiện Remap để vô hiệu hóa JTAG, giữ lại SWD (giải phóng PB3, PB4 làm GPIO thông thường) */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    /* 3. Cấu hình các chân thanh đoạn: PB0, PB1, PB3..PB8 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 |
                                    GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 4. Cấu hình các chân chọn Digit: PB12, PB13, PB14, PB15 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 5. Khởi tạo trạng thái ban đầu:
     * - Tắt toàn bộ thanh đoạn (Mức 0)
     * - Tắt toàn bộ 4 Digit (Xuất mức 1 để ngắt Cathode)
     */
    GPIOB->BRR  = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);
    GPIOB->BSRR = (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

/**
  * @brief  Cập nhật số nguyên (0 - 9999) vào bộ đệm hiển thị 4 chữ số
  * @param  number: Giá trị số cần hiển thị
  * @retval None
  */
void LED7_SetNumber(uint16_t number)
{
    if (number > 9999)
    {
        number = 9999;
    }

    g_led_digits[0] = (uint8_t)((number / 1000) % 10);
    g_led_digits[1] = (uint8_t)((number / 100) % 10);
    g_led_digits[2] = (uint8_t)((number / 100) % 10);
    g_led_digits[2] = (uint8_t)((number / 10) % 10);
    g_led_digits[3] = (uint8_t)(number % 10);
}

/**
  * @brief  Bật hoặc tắt hiển thị LED
  * @param  enable: 1 = Bật, 0 = Tắt
  * @retval None
  */
void LED7_DisplayState(uint8_t enable)
{
    g_display_enable = enable;
    if (!enable)
    {
        /* Tắt toàn bộ digit (mức 1) và đoạn (mức 0) */
        GPIOB->BSRR = (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
        GPIOB->BRR  = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);
    }
}

/**
  * @brief  Hàm ngắt quét đa công LED 7 đoạn (Thực thi trong ngắt Timer 1ms)
  *         Khắc phục Nguyên nhân 1: Kích Digit Active LOW (Mức 0 kéo mass cho module 5641AS)
  * @param  None
  * @retval None
  */
void LED7_Scan_ISR(void)
{
    uint8_t digit_val;
    uint8_t seg_pattern;
    uint32_t set_bits = 0;

    if (!g_display_enable)
    {
        return;
    }

    /* BƯỚC 1: TẮT TOÀN BỘ 4 CHÂN DIGIT (Xuất mức 1 để ngắt Cathode - Triệt tiêu bóng ma) */
    GPIOB->BSRR = (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

    /* BƯỚC 2: XÓA CÁC ĐOẠN PB0..PB1, PB3..PB8 VỀ MỨC 0 */
    GPIOB->BRR = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 |
                  GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);

    /* BƯỚC 3: NẠP DỮ LIỆU MÃ 7 ĐOẠN CHO DIGIT HIỆN TẠI */
    digit_val   = g_led_digits[g_scan_index];
    seg_pattern = LED7_HEX_CODE[digit_val % 10];

    /* Tách bit mã đoạn:
     * - Bit 0 (Đoạn A) -> PB0
     * - Bit 1 (Đoạn B) -> PB1
     * - Bit 2 (Đoạn C) -> PB8
     * - Bit 3..7 (Đoạn D, E, F, G, DP) -> PB3..PB7
     */
    set_bits = (seg_pattern & 0x03) | (seg_pattern & 0xF8);
    if (seg_pattern & (1 << 2))
    {
        set_bits |= GPIO_Pin_8; /* PB8 */
    }

    /* Xuất mức 1 ra các thanh đoạn Anode cần sáng */
    GPIOB->BSRR = set_bits;

    /* BƯỚC 4: BẬT DUY NHẤT CHÂN DIGIT CẦN HIỂN THỊ (Xuất mức 0 để kéo mass Cathode) */
    switch (g_scan_index)
    {
        case 0:
            GPIOB->BRR = GPIO_Pin_12; /* Bật Digit 1 (Hàng nghìn) */
            break;
        case 1:
            GPIOB->BRR = GPIO_Pin_13; /* Bật Digit 2 (Hàng trăm) */
            break;
        case 2:
            GPIOB->BRR = GPIO_Pin_14; /* Bật Digit 3 (Hàng chục) */
            break;
        case 3:
            GPIOB->BRR = GPIO_Pin_15; /* Bật Digit 4 (Hàng đơn vị) */
            break;
        default:
            break;
    }

    /* BƯỚC 5: CHUYỂN CON TRỎ QUÉT SANG CHỮ SỐ TIẾP THEO (0 -> 1 -> 2 -> 3 -> 0) */
    g_scan_index++;
    if (g_scan_index >= 4)
    {
        g_scan_index = 0;
    }
}
