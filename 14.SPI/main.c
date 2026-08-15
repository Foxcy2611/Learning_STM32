// ============================================================
// main.c - SPI1 Driver (SPL) cho STM32F103C8 - Master mode, polling
// PA4 = NSS 
// PA5 = SCK
// PA6 = MISO
// PA7 = MOSI
// ============================================================

// RFID ; TFT

#include "stm32f10x.h"
#include "stm32f10x_spi.h"              // Device:StdPeriph Drivers:SPI

#define SPI1_CS_PORT   GPIOA
#define SPI1_CS_PIN    GPIO_Pin_4

#define SPI1_CS_LOW()   GPIO_ResetBits(SPI1_CS_PORT, SPI1_CS_PIN)
#define SPI1_CS_HIGH()  GPIO_SetBits(SPI1_CS_PORT, SPI1_CS_PIN)

void SPI1_Init(void);
uint8_t SPI1_TransferByte(uint8_t data);
void SPI1_WriteReg(uint8_t regAddr, uint8_t data);
uint8_t SPI1_ReadReg(uint8_t regAddr);
void SPI1_ReadBytes(uint8_t regAddr, uint8_t *pBuf, uint16_t len);
void SPI1_WriteBytes(uint8_t regAddr, uint8_t *pBuf, uint16_t len);

void SPI1_Init(void){
	GPIO_InitTypeDef gpio;
	SPI_InitTypeDef  spi;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

	// SCK, MOSI: Alternate Function Push-Pull (do MCU dieu khien)
	gpio.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_7;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);

	// MISO: Input Floating (do slave dieu khien muc, MCU chi doc) 
	gpio.GPIO_Pin  = GPIO_Pin_6;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpio);

	// CS: Dieu khien bang tay - quyet dinh chon SLAVE
	gpio.GPIO_Pin   = SPI1_CS_PIN;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(SPI1_CS_PORT, &gpio);
	SPI1_CS_HIGH();  // mac dinh nho CS (chua chon slave)

	SPI_I2S_DeInit(SPI1);
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_CPOL = SPI_CPOL_Low;   // Mode 0: CPOL=0, CPHA=0 - pho bien nhat
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_NSS = SPI_NSS_Soft;  
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &spi);
	SPI_Cmd(SPI1, ENABLE);
}

// Truyen + nhan dong thoi 1 byte
uint8_t SPI1_TransferByte(uint8_t data){
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
	SPI_I2S_SendData(SPI1, data);

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI1);
}

int main(void){
	uint8_t whoAmI;
	uint8_t buf[6];

	SPI1_Init();

	// Vi du doc thanh ghi WHO_AM_I, vi du reg 0x0F (kieu cam bien SPI nhu BMP280/LIS3DH)
	whoAmI = SPI1_ReadReg(0x0F);

	// Vi du ghi cau hinh vao reg 0x20
	SPI1_WriteReg(0x20, 0x57);

	// Vi du doc 6 byte du lieu lien tiep tu reg 0x28
	SPI1_ReadBytes(0x28, buf, 6);

	while (1){
		if(whoAmI > 0){
			
		}
	}
}
