#ifndef __FLASH_LOGGER_H
#define __FLASH_LOGGER_H

#include "stm32f10x.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Địa chỉ bắt đầu của Page 63 (Page cuối của 64KB Flash nội) */
#define FLASH_LOG_PAGE_ADDR       ((uint32_t)0x0800FC00)

/* Dung lượng 1 Page Flash trên STM32F103C8T6 là 1024 Bytes */
#define FLASH_PAGE_SIZE_BYTES     ((uint32_t)1024)

/* Số mẫu lưu tối đa trong bộ đệm vòng (Đề bài yêu cầu ~50 mẫu) */
#define FLASH_LOG_MAX_SAMPLES     (50)

/* Giá trị ô nhớ Flash chưa ghi dữ liệu (mặc định sau Erase) */
#define FLASH_EMPTY_VALUE         ((uint16_t)0xFFFF)

/* Khởi tạo hệ thống lưu trữ Flash, tìm vị trí ghi tiếp theo sau khi Reset */
void FLASH_Logger_Init(void);

/* Ghi 1 mẫu giá trị ADC vào Flash theo cơ chế Append/Circular Buffer */
uint8_t FLASH_Logger_WriteSample(uint16_t adc_val);

/* Đọc giá trị của mẫu tại chỉ số cụ thể (0 đến GetSampleCount-1) */
uint16_t FLASH_Logger_ReadSample(uint16_t index);

/* Lấy tổng số lượng mẫu hiện đang được lưu trong Flash */
uint16_t FLASH_Logger_GetSampleCount(void);

/* Xóa trắng toàn bộ vùng nhớ Log (Page 63) */
void FLASH_Logger_ClearAll(void);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_LOGGER_H */
