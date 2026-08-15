#ifndef __BKP__H
#define __BKP__H

#include "stm32f10x.h"

// --- KHAI BAO HAM CHO BKP ---
void BKP_Init_Access(void);
void BKP_WriteData(uint16_t bkp_register, uint16_t data);
uint16_t BKP_ReadData(uint16_t bkp_register);

#endif 
