#include "LCD.h"

extern void delay_ms(uint16_t cnt);
extern void delay_us(uint16_t cnt);

void LCD_Send4Bit(uint8_t data, uint8_t rs)
{
    uint8_t temp = 0;

    /* Data -> D4-D7 */
    temp |= ((data >> 0) & 0x01) << LCD_D4;
    temp |= ((data >> 1) & 0x01) << LCD_D5;
    temp |= ((data >> 2) & 0x01) << LCD_D6;
    temp |= ((data >> 3) & 0x01) << LCD_D7;

    /* Backlight ON */
    temp |= (1 << LCD_BL);

    /* RS */
    if (rs)
        temp |= (1 << LCD_RS);

    /* RW = 0: Write */
    temp &= ~(1 << LCD_RW);

    /* E = 1 */
    temp |= (1 << LCD_EN);
    PCF8574_Write(temp);

    delay_us(1);

    /* E = 0 */
    temp &= ~(1 << LCD_EN);
    PCF8574_Write(temp);

    delay_us(1);
}

void LCD_SendCommand(uint8_t cmd)
{
    /* RS = 0 */
    LCD_Send4Bit(cmd >> 4, 0);
    LCD_Send4Bit(cmd & 0x0F, 0);

    delay_us(50);
}


void LCD_SendData(uint8_t data)
{
    /* RS = 1 */
    LCD_Send4Bit(data >> 4, 1);
    LCD_Send4Bit(data & 0x0F, 1);

    delay_us(50);
}

void LCD_Init(void)
{
    /* Ch? ngu?n ?n d?nh sau khi c?p di?n (>40ms theo datasheet) */
    delay_ms(50);

    /* --- B?t bu?c ph?i g?i 3 l?n l?nh 0x3 (d?ng 8-bit) d? reset v? tr?ng thái bi?t tru?c ---
     * Luu ý: ? bu?c này LCD v?n dang ? ch? d? 8-bit nên ta ch? g?i nibble cao,
     * không g?i LCD_SendCommand() vì hàm dó g?i c? 2 nibble (dành cho 4-bit mode).
     */
    LCD_Send4Bit(0x03, 0);
    delay_ms(5);   // d?i > 4.1ms

    LCD_Send4Bit(0x03, 0);
    delay_us(150); // d?i > 100us

    LCD_Send4Bit(0x03, 0);
    delay_us(150);

    /* Chuy?n sang ch? d? giao ti?p 4-bit */
    LCD_Send4Bit(0x02, 0);
    delay_us(150);

    /* T? dây LCD dã ? 4-bit mode, có the dùng LCD_SendCommand bình thuong */

    /* Function set: 4-bit, 2 dòng, font 5x8 */
    LCD_SendCommand(0x28);

    /* Display control: t?t display tru?c khi c?u hình */
    LCD_SendCommand(0x08);

    /* Clear display */
    LCD_SendCommand(0x01);
    delay_ms(2); // l?nh Clear c?n th?i gian th?c thi lâu hon (~1.6-2ms)

    /* Entry mode set: tang con tro, không dich màn hình */
    LCD_SendCommand(0x06);

    /* Display control: bat display, tat cursor, tat blink */
    LCD_SendCommand(0x0C);

    /* Clear l?i l?n n?a cho ch?c (tu? ch?n) */
    LCD_SendCommand(0x01);
    delay_ms(2);
}

void LCD_Clear(void)
{
    LCD_SendCommand(0x01);  // Clear display
    delay_ms(2);            // L?nh này c?n th?i gian th?c thi lâu hon (~1.6-2ms)
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    /* Ð?a ch? DDRAM b?t d?u c?a t?ng dòng (LCD 16x2 ch? dùng row 0-1) */
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};

    if (row > 3) row = 3; // gi?i h?n an toàn (LCD 20x4 t?i da 4 dòng)

    LCD_SendCommand(0x80 | (col + row_offsets[row]));
}

void LCD_Print(const char *str)
{
    while (*str)
    {
        LCD_SendData((uint8_t)(*str));
        str++;
    }
}
