// ============================================================
// main.c - I2C1 Driver (SPL) cho STM32F103C8 - Master mode, polling
// PB6: SCL
// PB7: SDA
// ============================================================

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"              // Device:StdPeriph Drivers:I2C

void I2C1_Init(void);
void I2C1_WriteReg(uint8_t devAddr, uint8_t regAddr, uint8_t data);
void I2C1_WriteBytes(uint8_t devAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len);
void I2C1_ReadReg(uint8_t devAddr, uint8_t regAddr, uint8_t *pData);
void I2C1_ReadBytes(uint8_t devAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len);

#define I2C1_SCL GPIO_Pin_6
#define I2C1_SDA GPIO_Pin_7

void I2C1_Init(void){
  GPIO_InitTypeDef gpio;
  I2C_InitTypeDef i2c;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  gpio.GPIO_Pin = I2C1_SCL | I2C1_SDA;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &gpio);

  I2C_DeInit(I2C1);
  i2c.I2C_Mode = I2C_Mode_I2C;
  i2c.I2C_DutyCycle = I2C_DutyCycle_2;
  i2c.I2C_OwnAddress1 = 0x00;
  i2c.I2C_Ack = I2C_Ack_Enable;
  i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  i2c.I2C_ClockSpeed = 100000;   // Co the 400k neu slave ho tro Fast mode
  I2C_Init(I2C1, &i2c);

  I2C_Cmd(I2C1, ENABLE);
}

// Ghi 1 byte vao 1 thanh ghi cua slave
// START -> dia chi (write) -> regAddr -> data -> STOP
void I2C1_WriteReg(uint8_t devAddr, uint8_t regAddr, uint8_t data){
  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  I2C_SendData(I2C1, regAddr);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  I2C_SendData(I2C1, data);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  I2C_GenerateSTOP(I2C1, ENABLE);
}

// Ghi nhieu byte lien tiep 
void I2C1_WriteBytes(uint8_t devAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len){
  uint16_t i;

  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  I2C_SendData(I2C1, regAddr);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  for (i = 0; i < len; i++){
    I2C_SendData(I2C1, pBuf[i]);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  }

  I2C_GenerateSTOP(I2C1, ENABLE);
}

// Doc 1 byte tu 1 thanh ghi cua slave
void I2C1_ReadReg(uint8_t devAddr, uint8_t regAddr, uint8_t *pData){
  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  I2C_SendData(I2C1, regAddr);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  I2C_GenerateSTART(I2C1, ENABLE); // Restart
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  I2C_AcknowledgeConfig(I2C1, DISABLE); // Doc 1 byte -> NACK ngay de bao slave dung sau byte nay

  I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Receiver);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

  I2C_GenerateSTOP(I2C1, ENABLE);

  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
  *pData = I2C_ReceiveData(I2C1);

  I2C_AcknowledgeConfig(I2C1, ENABLE); // Bat lai ACK cho lan giao tiep sau
}

// Doc nhieu byte lien tiep tu regAddr
void I2C1_ReadBytes(uint8_t devAddr, uint8_t regAddr, uint8_t *pBuf, uint16_t len){
  uint16_t i;

  if (len == 0) return;

  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  I2C_SendData(I2C1, regAddr);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  I2C_GenerateSTART(I2C1, ENABLE); // Repeat START 
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  I2C_AcknowledgeConfig(I2C1, ENABLE);

  I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Receiver);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

  for (i = 0; i < len; i++){
    if (i == len - 1){
        I2C_AcknowledgeConfig(I2C1, DISABLE); // Gui NACK
        I2C_GenerateSTOP(I2C1, ENABLE);
    }

    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
    pBuf[i] = I2C_ReceiveData(I2C1);
  }

  I2C_AcknowledgeConfig(I2C1, ENABLE);
}

int main(void){
  uint8_t whoAmI;
  uint8_t buf[6];

  I2C1_Init();

  I2C1_ReadReg(0x68 << 1, 0x75, &whoAmI);

  I2C1_WriteReg(0x68 << 1, 0x6B, 0x00);

  I2C1_ReadBytes(0x68 << 1, 0x3B, buf, 6);

  while (1){
    
  }
}
