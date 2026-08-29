#ifndef __LED7_H
#define __LED7_H

#include "stm32f10x.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Khởi tạo GPIO điều khiển LED 7 đoạn và Remap tắt JTAG để dùng PB3, PB4 */
void LED7_Init(void);

/* Cập nhật số cần hiển thị (0 - 9999) */
void LED7_SetNumber(uint16_t number);

/* Hàm quét hiển thị đa công (gọi trong ngắt định kỳ 1ms của Timer) */
void LED7_Scan_ISR(void);

/* Bật hoặc tắt toàn bộ LED */
void LED7_DisplayState(uint8_t enable);

#ifdef __cplusplus
}
#endif

#endif /* __LED7_H */
