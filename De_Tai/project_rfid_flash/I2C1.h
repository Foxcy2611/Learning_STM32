#ifndef I2C1_H
#define I2C1_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"

#define I2C1_SCL GPIO_Pin_6 
#define I2C1_SDA GPIO_Pin_7
#define devAddr (0x27 << 1)

void I2C1_Init(void);
void I2C1_WriteByte(uint8_t data);

#endif