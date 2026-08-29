#ifndef __FLASH_LOGGER_H
#define __FLASH_LOGGER_H

#include "stm32f10x.h"

#ifdef __cplusplus
extern "C" {
#endif

// Page 63 - page cuoi cung cua Flash 64KB
#define FLASH_LOG_PAGE_ADDR       ((uint32_t)0x0800FC00)
#define FLASH_PAGE_SIZE_BYTES     ((uint32_t)1024)
#define FLASH_LOG_MAX_SAMPLES     (50)       // Toi da 50 mau
#define FLASH_EMPTY_VALUE         ((uint16_t)0xFFFF)

void     FLASH_Logger_Init(void);
uint8_t  FLASH_Logger_WriteSample(uint16_t adc_val);
uint16_t FLASH_Logger_ReadSample(uint16_t index);
uint16_t FLASH_Logger_GetSampleCount(void);
void     FLASH_Logger_ClearAll(void);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_LOGGER_H */
