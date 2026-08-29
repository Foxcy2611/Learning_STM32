#ifndef __LED7_H
#define __LED7_H

#include "stm32f10x.h"

#ifdef __cplusplus
extern "C" {
#endif

void LED7_Init(void);
void LED7_SetNumber(uint16_t number);
void LED7_Scan_ISR(void);
void LED7_DisplayState(uint8_t enable);

#ifdef __cplusplus
}
#endif

#endif /* __LED7_H */
