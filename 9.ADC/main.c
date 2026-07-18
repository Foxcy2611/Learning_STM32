#include "stm32f10x.h"                  // Device header
#include "stm32f10x_adc.h"              // Device:StdPeriph Drivers:ADC

// Khai bao mau cac ham
void ADC_Config(void); // Cau hinh GPIO + ADC
uint16_t ADC_Read(void);
void LED_Config(void);

// Doc ADC chan PA0
void ADC_Config(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

	ADC_InitTypeDef adc;
	GPIO_InitTypeDef gpio;
	
	// Cau hinh PA0 truoc
	gpio.GPIO_Mode = GPIO_Mode_AIN; 
	gpio.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &gpio);
	
	// Cau hinh ADC
	adc.ADC_ContinuousConvMode = ENABLE;
	adc.ADC_DataAlign = ADC_DataAlign_Right; // Ghi gia tri vao thanh ghi tu phai qua trai
	adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adc.ADC_Mode = ADC_Mode_Independent;
	adc.ADC_NbrOfChannel = 1; // Co bao nhieu kenh dc su dung
	adc.ADC_ScanConvMode = DISABLE;
	ADC_Init(ADC1, &adc);
	
	ADC_RegularChannelConfig(
		ADC1, // ADCx ?
		ADC_Channel_0, // Channel ?
		1, // Do uu tien bat dau tu 1
		ADC_SampleTime_55Cycles5
	);
	
	ADC_Cmd(ADC1, ENABLE);
	
	// Hieu chinh ADC (Bat buoc de qua trinh do khong bi sai)
	ADC_ResetCalibration(ADC1); // Reset thanh ghi ve 0
	while(ADC_GetResetCalibrationStatus(ADC1)); // Cho reset ve 0
	ADC_StartCalibration(ADC1); // Bat dau do
	while(ADC_GetCalibrationStatus(ADC1));
}

void LED_Config(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef gpio;
	
	// Cau hinh led PA4: 0 tat - 1 bat
	gpio.GPIO_Mode = GPIO_Mode_Out_PP; 
	gpio.GPIO_Pin = GPIO_Pin_4;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}

uint16_t ADC_Read(void){
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	// Cho khi do xong (Kiem tra co EOC)
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	
	uint16_t adc_result = ADC_GetConversionValue(ADC1);
	
	// Xoa co
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	
	return adc_result;
}

// Delay lao - Ko chuan
void Delay(uint32_t ms){
	for(volatile uint32_t i = 0 ; i < ms ; i++){
		for(volatile uint32_t j = 0 ; j < 0x2aff ; j++);
	}
}

int main(){
	ADC_Config();
	
	while(1){
		uint16_t adc_raw = ADC_Read();
		
		// Neu adc_raw > 2048 -> bat den
		if(adc_raw > 2048){
			GPIO_SetBits(GPIOA, GPIO_Pin_4);
		} else {
			GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		}
		
		Delay(1000);
	}
}
