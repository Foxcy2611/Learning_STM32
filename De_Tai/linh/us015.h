#ifndef __US015_H
#define __US015_H

#include "stm32f10x.h"
#include "delay.h"

// Kh?i t?o c?u hình chân c?m cho c?m bi?n
void US015_Init(void);

// Hàm do và tr? v? kho?ng cách (don v?: cm)
float US015_GetDistance(void);

#endif