#ifndef SPI_H
#define SPI_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"

void SPI1_Init(void);
uint8_t SPI1_TransferByte(uint8_t data);


#endif