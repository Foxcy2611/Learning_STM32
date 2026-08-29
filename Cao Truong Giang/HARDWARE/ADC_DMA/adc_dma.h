#ifndef __ADC_DMA_H
#define __ADC_DMA_H

#include "stm32f10x.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Khởi tạo ngoại vi ADC1 và DMA1 Channel 1 */
void ADC_DMA_Init(void);

/* Lấy giá trị ADC thô đo được từ biến DMA (0 - 4095) */
uint16_t ADC_DMA_GetValue(void);

/* Quy đổi giá trị ADC thành phần trăm độ sáng (0 - 100%) */
uint8_t ADC_DMA_GetPercent(void);

/* Quy đổi giá trị ADC thành điện áp thực tế (mV: 0 - 3300) */
uint16_t ADC_DMA_GetVoltage_mV(void);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_DMA_H */
