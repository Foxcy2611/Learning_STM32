#ifndef __FLASH__H
#define __FLASH__H

#include "stm32f10x.h"

// CAU HINH FLASH CHO STM32F103C8T6 (64KB Flash)
// 1 trang (Page) = 1KB -> 64 Page (0 -> 63)
// Dia chi bat dau Page 63: 0x08000000 + (63 * 1024) = 0x0800FC00

#define FINAL_ADDRESS_FLASH 0x0800FC00

// --- KHAI BAO HAM CHO FLASH ---
void FLASH_WriteHalfWord(uint32_t address, uint16_t data);
uint16_t FLASH_ReadHalfWord(uint32_t address);

#endif 
