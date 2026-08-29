#include "RC522.h"

void RST_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin   = MFRC522_RST_PIN;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC522_RST_PORT, &gpio);

	MFRC522_RST_HIGH();   // RST = HIGH
}

void MFRC522_WriteRegister(uint8_t reg, uint8_t value)
{
    MFRC522_CS_LOW();

    SPI1_TransferByte((reg << 1) & 0x7E);
    SPI1_TransferByte(value);

    MFRC522_CS_HIGH();
}

uint8_t MFRC522_ReadRegister(uint8_t reg)
{
    uint8_t value;

    MFRC522_CS_LOW();

    SPI1_TransferByte((reg << 1) | 0x80);
    value = SPI1_TransferByte(0x00);

    MFRC522_CS_HIGH();

    return value;
}

uint8_t MFRC522_ReadVersion(void)
{
    return MFRC522_ReadRegister(VersionReg);
}

void MFRC522_Reset(void)
{
    MFRC522_WriteRegister(CommandReg, PCD_RESETPHASE);
}

void MFRC522_SetBitMask(uint8_t reg, uint8_t mask)  
{
    uint8_t tmp;
    tmp = MFRC522_ReadRegister(reg);
    MFRC522_WriteRegister(reg, tmp | mask);
}

void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask)  
{
    uint8_t tmp;
    tmp = MFRC522_ReadRegister(reg);
    MFRC522_WriteRegister(reg, tmp & (~mask));
}

void MFRC522_AntennaOn(void)
{
    uint8_t temp;

    temp = MFRC522_ReadRegister(TxControlReg);
    if (!(temp & 0x03))
    {
        MFRC522_SetBitMask(TxControlReg, 0x03);
    }
}

void MFRC522_AntennaOff(void)
{
    MFRC522_ClearBitMask(TxControlReg, 0x03);
}

void MFRC522_Init(void)
{
    MFRC522_Reset();
         
    MFRC522_WriteRegister(TModeReg, 0x8D);      // Tprescaler_Hi
    MFRC522_WriteRegister(TPrescalerReg, 0x3E); // Tprescaler_Lo
    MFRC522_WriteRegister(TReloadRegL, 30);      // Reload value L
    MFRC522_WriteRegister(TReloadRegH, 0);       // Reload value H
    
    MFRC522_WriteRegister(TxASKReg, 0x40);      // Force 100% ASK modulation
    MFRC522_WriteRegister(ModeReg, 0x3D);       // CRC initial value 0x6363
    
    MFRC522_AntennaOn();                        // Turn on antenna
}

uint8_t MFRC522_ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint16_t *backLen)
{
    uint8_t status = MI_ERR;
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint16_t i;

    if (command == PCD_AUTHENT)
    {
        irqEn = 0x12;
        waitIRq = 0x10;
    }
    if (command == PCD_TRANSCEIVE)
    {
        irqEn = 0x77;
        waitIRq = 0x30;
    }

    MFRC522_WriteRegister(CommIEnReg, irqEn | 0x80);
    MFRC522_ClearBitMask(CommIrqReg, 0x80);
    MFRC522_SetBitMask(FIFOLevelReg, 0x80);

    MFRC522_WriteRegister(CommandReg, PCD_IDLE);

    for (i = 0; i < sendLen; i++)
    {
        MFRC522_WriteRegister(FIFODataReg, sendData[i]);
    }

    MFRC522_WriteRegister(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {
        MFRC522_SetBitMask(BitFramingReg, 0x80);
    }

    i = 2000;
    do
    {
        n = MFRC522_ReadRegister(CommIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitIRq));

    MFRC522_ClearBitMask(BitFramingReg, 0x80);

    if (i != 0)
    {
        if (!(MFRC522_ReadRegister(ErrorReg) & 0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR;
            }

            if (command == PCD_TRANSCEIVE)
            {
                n = MFRC522_ReadRegister(FIFOLevelReg);
                lastBits = MFRC522_ReadRegister(ControlReg) & 0x07;
                if (lastBits)
                {
                    *backLen = (n - 1) * 8 + lastBits;
                }
                else
                {
                    *backLen = n * 8;
                }

                if (n == 0)
                {
                    n = 1;
                }
                if (n > 16)
                {
                    n = 16;
                }

                for (i = 0; i < n; i++)
                {
                    backData[i] = MFRC522_ReadRegister(FIFODataReg);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }
    }

    return status;
}

uint8_t MFRC522_Request(uint8_t reqMode, uint8_t *TagType)
{
    uint8_t status;  
    uint16_t backLen;
    uint8_t TagType_buffer[2];

    MFRC522_WriteRegister(BitFramingReg, 0x07);

    TagType_buffer[0] = reqMode;  
    status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType_buffer, 1, TagType_buffer, &backLen);

    if ((status == MI_OK) && (backLen == 16))
    {    
        TagType[0] = TagType_buffer[0];
        TagType[1] = TagType_buffer[1];
    }
    else
    {   
        status = MI_ERR;
    }
   
    return status;
}

uint8_t MFRC522_Anticoll(uint8_t *serNum)
{
    uint8_t status;
    uint8_t i;
    uint8_t serNumCheck = 0;
    uint16_t unLen;

    MFRC522_WriteRegister(BitFramingReg, 0x00);

    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
    {
        for (i = 0; i < 4; i++)
        {
            serNumCheck ^= serNum[i];
        }
        if (serNumCheck != serNum[i])
        {   
            status = MI_ERR;    
        }
    }

    return status;
}

void MFRC522_Halt(void)
{
    uint16_t unLen;
    uint8_t buff[4];

    buff[0] = PICC_HALT;
    buff[1] = 0;
    buff[2] = 0x57;
    buff[3] = 0xCD;
    MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}

uint8_t MFRC522_Check(uint8_t *id)
{
    uint8_t status;
    uint8_t type[2];

    status = MFRC522_Request(PICC_REQIDL, type);
    if (status == MI_OK)
    {
        status = MFRC522_Anticoll(id);
    }
    else
    {
        status = MI_ERR;
    }

    MFRC522_Halt(); // Sleep tag after reading

    return status;
}