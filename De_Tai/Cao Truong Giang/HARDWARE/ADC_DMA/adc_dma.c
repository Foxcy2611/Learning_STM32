#include "adc_dma.h"

// Bien luu gia tri ADC doc ve tu DMA
static volatile uint16_t g_adc_raw_value = 0;

// Khoi tao PA0 (ADC1_IN0) va DMA1 Channel 1
void ADC_DMA_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef  ADC_InitStructure;
    DMA_InitTypeDef  DMA_InitStructure;

    // Bat clock cho GPIOA, ADC1 va DMA1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // Cau hinh chan PA0 o che do Analog Input
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Cau hinh DMA1 Channel 1 doc tu ADC1->DR vao bien
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)&g_adc_raw_value;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = 1;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    // Cau hinh ADC1
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ADCCLK = 72 / 6 = 12MHz

    ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode       = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel       = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);

    // Cho phep ADC kich hoat DMA
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    // Calib ADC
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

// Doc 1 mau ADC bang DMA
static uint16_t ADC_DMA_ReadSingle(void)
{
    DMA_Cmd(DMA1_Channel1, DISABLE);
    DMA_SetCurrDataCounter(DMA1_Channel1, 1);
    DMA_ClearFlag(DMA1_FLAG_TC1);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
    DMA_ClearFlag(DMA1_FLAG_TC1);

    return g_adc_raw_value;
}

// Lay trung binh 16 mau ADC de loc nhieu
uint16_t ADC_DMA_GetValue(void)
{
    uint32_t sum = 0;
    uint8_t i;

    for (i = 0; i < 16; i++)
    {
        sum += ADC_DMA_ReadSingle();
    }

    return (uint16_t)(sum / 16);
}

// Quy doi ADC sang % (0 - 100%)
uint8_t ADC_DMA_GetPercent(void)
{
    uint16_t val = ADC_DMA_GetValue();
    return (uint8_t)(((uint32_t)val * 100) / 4095);
}

// Quy doi ADC sang mV (0 - 3300mV)
uint16_t ADC_DMA_GetVoltage_mV(void)
{
    uint16_t val = ADC_DMA_GetValue();
    return (uint16_t)(((uint32_t)val * 3300) / 4095);
}
