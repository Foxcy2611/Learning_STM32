#include "PCF8574.h"

void PCF8574_Write(uint8_t data){
	I2C1_WriteByte(data);
}