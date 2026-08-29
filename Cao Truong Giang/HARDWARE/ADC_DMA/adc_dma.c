#include "adc_dma.h"

/* Biến toàn cục lưu kết quả chuyển đổi ADC do DMA ghi vào */
static volatile uint16_t g_adc_raw_value = 0;

/**
  * @brief  Khởi tạo GPIO PA0, ADC1 và DMA1 Channel 1 ở chế độ Normal Mode
  * @param  None
  * @retval None
  */
void ADC_DMA_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef  ADC_InitStructure;
    DMA_InitTypeDef  DMA_InitStructure;

    /* 1. Cấp xung nhịp cho GPIOA, ADC1 và DMA1 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* 2. Cấu hình chân PA0 làm ngõ vào tương tự (Analog Input) */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 3. Cấu hình DMA1 Channel 1 */
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

    /* 4. Cấu hình ADC1 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode       = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel       = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);

    /* Cho phép ADC1 gửi yêu cầu DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* Bật ngoại vi ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /* 5. Hiệu chuẩn nội (Calibration) */
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

/**
  * @brief  Đọc 1 mẫu ADC thô qua DMA
  * @param  None
  * @retval Giá trị ADC 12-bit (0 - 4095)
  */
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

/**
  * @brief  Đọc giá trị ADC kết hợp BỘ LỌC TRUNG BÌNH ĐỘNG (Moving Average Filter 16 mẫu)
  *         Triệt tiêu hoàn toàn nhiễu rung lắc ở hàng đơn vị và hàng chục.
  * @param  None
  * @retval Giá trị ADC 12-bit đã qua lọc mượt (0 - 4095)
  */
uint16_t ADC_DMA_GetValue(void)
{
    uint32_t sum = 0;
    uint8_t i;

    /* Lấy tổng của 16 mẫu liên tiếp */
    for (i = 0; i < 16; i++)
    {
        sum += ADC_DMA_ReadSingle();
    }

    /* Tính trung bình cộng */
    return (uint16_t)(sum / 16);
}

/**
  * @brief  Quy đổi giá trị ADC thành phần trăm (0 - 100%)
  * @param  None
  * @retval Phần trăm mức sáng
  */
uint8_t ADC_DMA_GetPercent(void)
{
    uint16_t val = ADC_DMA_GetValue();
    return (uint8_t)(((uint32_t)val * 100) / 4095);
}

/**
  * @brief  Quy đổi giá trị ADC thành điện áp mV (0 - 3300mV)
  * @param  None
  * @retval Điện áp tại chân đo (mV)
  */
uint16_t ADC_DMA_GetVoltage_mV(void)
{
    uint16_t val = ADC_DMA_GetValue();
    return (uint16_t)(((uint32_t)val * 3300) / 4095);
}
