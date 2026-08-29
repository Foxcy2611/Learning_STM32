#include "SPI.h"

void SPI1_Init(void){
	GPIO_InitTypeDef gpio;
	SPI_InitTypeDef spi;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	// Configure PA5 (SCK) and PA7 (MOSI)
	gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);
	
	// Configure PA6 (MISO)
	gpio.GPIO_Pin  = GPIO_Pin_6;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &gpio);
	
	// Configure PA4 (CS/NSS)
	gpio.GPIO_Pin   = GPIO_Pin_4;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &gpio);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	
	SPI_I2S_DeInit(SPI1);
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // 72MHz/256 � 281kHz
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CRCPolynomial = 7;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1, &spi);
	SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_TransferByte(uint8_t data) {
    uint32_t timeout = 0xFFFF; // Sử dụng một biến đếm vừa đủ

    // 1. Chờ cờ TXE (Bộ đệm truyền trống) với Timeout
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {
        if (--timeout == 0) {
            // Xử lý khi quá thời gian chờ (ví dụ: thoát hàm và trả về 0)
            return 0; 
        }
    }
    
    // Gửi dữ liệu đi
    SPI_I2S_SendData(SPI1, data);
    
    timeout = 0xFFFF;
    // 2. Chờ cờ RXNE (Có dữ liệu nhận về) với Timeout
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {
        if (--timeout == 0) {
            // Nếu bị timeout, kiểm tra xem có phải do lỗi tràn đệm (OVR) không
            if (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_OVR) == SET) {
                // Để xóa cờ OVR trên STM32: Đọc thanh ghi DR rồi đọc thanh ghi SR (hoặc ngược lại)
                SPI_I2S_ReceiveData(SPI1);
                SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_OVR); 
            }
            return 0; // Tránh treo CPU, thoát ra ngoài
        }
    }
    
    // Trả về dữ liệu nhận được
    return SPI_I2S_ReceiveData(SPI1);
}
