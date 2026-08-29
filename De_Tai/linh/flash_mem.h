#ifndef __FLASH_MEM_H
#define __FLASH_MEM_H

#include "stm32f10x.h"

// Page 63
#define FLASH_USER_START_ADDR   ((uint32_t)0x0800FC00)   


void Flash_WriteThreshold(float value);

float Flash_ReadThreshold(void);

#endif