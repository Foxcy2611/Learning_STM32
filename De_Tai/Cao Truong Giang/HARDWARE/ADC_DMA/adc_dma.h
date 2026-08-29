#ifndef __ADC_DMA_H
#define __ADC_DMA_H

#include "stm32f10x.h"

#ifdef __cplusplus
extern "C" {
#endif

void ADC_DMA_Init(void);
uint16_t ADC_DMA_GetValue(void);        // Doc gia tri ADC trung binh (0 - 4095)
uint8_t  ADC_DMA_GetPercent(void);     // Quy doi ra % (0 - 100)
uint16_t ADC_DMA_GetVoltage_mV(void);  // Quy doi ra dien ap mV (0 - 3300mV)

#ifdef __cplusplus
}
#endif

#endif /* __ADC_DMA_H */
