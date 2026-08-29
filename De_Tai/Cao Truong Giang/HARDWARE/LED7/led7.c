#include "led7.h"

// Ma 7 doan Cathode chung (0 - 9)
static const uint8_t LED7_HEX_CODE[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

// Mang luu 4 so can hien thi
static volatile uint8_t g_led_digits[4] = {0, 0, 0, 0};
static volatile uint8_t g_scan_index = 0;
static volatile uint8_t g_display_enable = 1;

// Khoi tao cac chan GPIO cho LED 7 doan
// Doan a..g: PB0, PB1, PB8, PB3..PB7 (muc 1 sang)
// Chon digit: PB12..PB15 (muc 0 sang)
void LED7_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    // Tat JTAG de dung PB3, PB4 lam GPIO
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    // Cau hinh cac chan doan (segment)
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 |
                                    GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Cau hinh cac chan chon digit
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Ban dau tat het LED
    GPIOB->BRR  = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);
    GPIOB->BSRR = (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

// Tach so thanh 4 chu so de hien thi
void LED7_SetNumber(uint16_t number)
{
    if (number > 9999)
    {
        number = 9999;
    }

    g_led_digits[0] = (uint8_t)((number / 1000) % 10);
    g_led_digits[1] = (uint8_t)((number / 100) % 10);
    g_led_digits[2] = (uint8_t)((number / 10) % 10);
    g_led_digits[3] = (uint8_t)(number % 10);
}

// Bat / tat man hinh LED
void LED7_DisplayState(uint8_t enable)
{
    g_display_enable = enable;
    if (!enable)
    {
        GPIOB->BSRR = (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
        GPIOB->BRR  = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);
    }
}

// Ham quet tung LED (goi trong ngat Timer)
void LED7_Scan_ISR(void)
{
    uint8_t digit_val;
    uint8_t seg_pattern;
    uint32_t set_bits = 0;

    if (!g_display_enable)
    {
        return;
    }

    // Tat ca 4 digit truoc de chong lem (ghosting)
    GPIOB->BSRR = (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

    // Xoa cac doan
    GPIOB->BRR = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 |
                  GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);

    digit_val   = g_led_digits[g_scan_index];
    seg_pattern = LED7_HEX_CODE[digit_val % 10];

    // Map bit ma 7 doan ra cac chan PB
    // Bit 0 (A)->PB0, Bit 1 (B)->PB1, Bit 2 (C)->PB8, Bit 3..7->PB3..PB7
    set_bits = (seg_pattern & 0x03) | (seg_pattern & 0xF8);
    if (seg_pattern & (1 << 2))
    {
        set_bits |= GPIO_Pin_8;
    }

    // Xuat muc 1 vao cac doan can sang
    GPIOB->BSRR = set_bits;

    // Bat digit tuong ung (keo ve 0)
    switch (g_scan_index)
    {
        case 0:
            GPIOB->BRR = GPIO_Pin_12; // Digit 1
            break;
        case 1:
            GPIOB->BRR = GPIO_Pin_13; // Digit 2
            break;
        case 2:
            GPIOB->BRR = GPIO_Pin_14; // Digit 3
            break;
        case 3:
            GPIOB->BRR = GPIO_Pin_15; // Digit 4
            break;
        default:
            break;
    }

    // Chuyen sang digit tiep theo
    g_scan_index++;
    if (g_scan_index >= 4)
    {
        g_scan_index = 0;
    }
}
