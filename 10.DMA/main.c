#include "stm32f10x.h"                  // Device header
#include "stm32f10x_adc.h"              // Device:StdPeriph Drivers:ADC
#include "stm32f10x_dma.h"              // Device:StdPeriph Drivers:DMA

// Ngoai vi <-> Bo nho

/* 
- DB
+ Doc gia tri ADC tu chan PA0
+ Ho tro DMA
+ Dua gia tri ADC ma bat/tat led PC13
*/

static uint16_t adc_raw = 0;

void GPIO_ADC_Config(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef gpio;
	ADC_InitTypeDef adc;
	
	// Cau hinh LED PC13 
	gpio.GPIO_Mode = GPIO_Mode_Out_PP; 
	gpio.GPIO_Pin = GPIO_Pin_13;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	
	adc.ADC_ContinuousConvMode = ENABLE;
	adc.ADC_DataAlign = ADC_DataAlign_Right;
	adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adc.ADC_Mode = ADC_Mode_Independent;
	adc.ADC_NbrOfChannel = 1;
	adc.ADC_ScanConvMode = DISABLE;
	ADC_Init(ADC1, &adc);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	
	ADC_DMACmd(ADC1, ENABLE);
	
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
}

void DMA_Config(void){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_InitTypeDef dma;
	
	DMA_DeInit(DMA1_Channel1);
	
	dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	dma.DMA_MemoryBaseAddr = (uint32_t)&adc_raw;
	dma.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma.DMA_BufferSize = 1;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma.DMA_Mode = DMA_Mode_Normal;
	dma.DMA_Priority = DMA_Priority_High;
	dma.DMA_M2M = DMA_M2M_Enable;
	DMA_Init(DMA1_Channel1, &dma);
		
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
	DMA_ClearFlag(DMA1_FLAG_TC1);
	
	DMA_Cmd(DMA1, ENABLE);
}

int main(){
	GPIO_ADC_Config();
	DMA_Config();
	
	if(adc_raw > 2000){
		// Bat den
		// delay
	} else {
		// Tat den
		// delay
	}
}
