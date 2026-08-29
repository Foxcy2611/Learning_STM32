#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_i2c.h"
#define OLed (0x3C << 1)
#define CLK GPIO_Pin_6
#define SDA GPIO_Pin_7
#include "string.h"
#include "stdio.h"
#define ngang 4
#define doc 4
#define dai 20
void Config(){
	GPIO_InitTypeDef gp;
	GPIO_InitTypeDef gpio;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);

  gp.GPIO_Mode = GPIO_Mode_Out_PP;
gp.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
gp.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA , &gp);

 gpio.GPIO_Mode =  GPIO_Mode_IPU;
gpio.GPIO_Pin = 	GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA , &gpio);
}
void UART_Config(void){
	USART_InitTypeDef uart;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
	uart.USART_BaudRate = 9600;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Tx;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1 ,&uart);
	
	USART_Cmd(USART1 ,ENABLE);
	
}
void I2C_Config(void){
	GPIO_InitTypeDef gpio;
	I2C_InitTypeDef i2c;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	gpio.GPIO_Pin = CLK | SDA;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB , &gpio);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1 , ENABLE);
	i2c.I2C_Mode =I2C_Mode_I2C;
	i2c.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c.I2C_Ack = I2C_Ack_Enable;
	i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	i2c.I2C_ClockSpeed = 100000;
	i2c.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C1 , &i2c);
	
	I2C_Cmd(I2C1, ENABLE);
}
static  void I2C1_WriteByte(uint8_t gui , uint8_t nhan, uint8_t *so , uint16_t len){
	while(I2C_GetFlagStatus(I2C1 , I2C_FLAG_BUSY));
	uint16_t i;
	I2C_GenerateSTART(I2C1 ,ENABLE);
	while(!I2C_CheckEvent(I2C1 , I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1 ,gui , I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, nhan);
	while(!I2C_CheckEvent(I2C1 , I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	for(i =0 ;i<len ;i++){
		I2C_SendData(I2C1 , so[i]);
		while(! I2C_CheckEvent(I2C1 , I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	I2C_GenerateSTOP(I2C1 ,ENABLE);
}
void OLED_WriteCmd(uint8_t cmd){
	I2C1_WriteByte(OLed , 0x00 , &cmd , 1);
}
void  OLED_WriteData(uint8_t data){
	I2C1_WriteByte (OLed , 0x40 , &data , 1);
}
void OLED_WriteDataBuf(uint8_t *so , uint16_t len){
	I2C1_WriteByte(OLed , 0x40 , so , len);
}

void  OLED_TOADO(uint8_t y , uint8_t x){
	OLED_WriteCmd(0xB0 + y);
	OLED_WriteCmd(0x00 + (x& 0x0F));
	OLED_WriteCmd(0x10 + (x >> 4));
}
void OLED_Init(void){
	volatile uint32_t i;
	for(i =0 ;i< 2000000 ;i++);
OLED_WriteCmd(0xAE); 
    OLED_WriteCmd(0xD5); 
    OLED_WriteCmd(0x80);
    OLED_WriteCmd(0xA8); 
    OLED_WriteCmd(0x3F); 
    OLED_WriteCmd(0xD3);
OLED_WriteCmd(0x00);
    OLED_WriteCmd(0x40); 
    OLED_WriteCmd(0x8D); 
    OLED_WriteCmd(0x14); 
    OLED_WriteCmd(0x20);
    OLED_WriteCmd(0x00); 
    OLED_WriteCmd(0xA1); 
    OLED_WriteCmd(0xC8); 
    OLED_WriteCmd(0xDA); 
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0x81); 
    OLED_WriteCmd(0xCF);
    OLED_WriteCmd(0xD9); 
    OLED_WriteCmd(0xF1);
    OLED_WriteCmd(0xDB); 
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0xA4); 
    OLED_WriteCmd(0xA6); 
    OLED_WriteCmd(0xAF);
	}
void OLED_Clear(void){
	uint8_t y,x;
	for(y =0 ; y < 8 ; y++){
		OLED_TOADO(y,0);
		for( x  =0 ; x< 128 ;x++){
			OLED_WriteData(0x00);
	}
}
}
uint8_t chuso[][5] ={
 {0x00,0x00,0x00,0x00,0x00}, // (space) 0x20
    {0x00,0x00,0x5F,0x00,0x00}, // !
    {0x00,0x07,0x00,0x07,0x00}, // "
    {0x14,0x7F,0x14,0x7F,0x14}, // #
    {0x24,0x2A,0x7F,0x2A,0x12}, // $
    {0x23,0x13,0x08,0x64,0x62}, // %
    {0x36,0x49,0x56,0x20,0x50}, // &
    {0x00,0x08,0x07,0x03,0x00}, // '
    {0x00,0x1C,0x22,0x41,0x00}, // (
    {0x00,0x41,0x22,0x1C,0x00}, // )
    {0x2A,0x1C,0x7F,0x1C,0x2A}, // *
    {0x08,0x08,0x3E,0x08,0x08}, // +
    {0x00,0x80,0x70,0x30,0x00}, // ,
    {0x08,0x08,0x08,0x08,0x08}, // -
    {0x00,0x00,0x60,0x60,0x00}, // .
    {0x20,0x10,0x08,0x04,0x02}, // /
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
    {0x00,0x36,0x36,0x00,0x00}, // :
    {0x00,0x56,0x36,0x00,0x00}, // ;
    {0x08,0x14,0x22,0x41,0x00}, // <
    {0x14,0x14,0x14,0x14,0x14}, // =
    {0x00,0x41,0x22,0x14,0x08}, // >
    {0x02,0x01,0x51,0x09,0x06}, // ?
    {0x32,0x49,0x79,0x41,0x3E}, // @
    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x09,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x7A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x06}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x7F,0x20,0x18,0x20,0x7F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x03,0x04,0x78,0x04,0x03}, // Y
    {0x61,0x51,0x49,0x45,0x43}, // Z
    {0x00,0x00,0x7F,0x41,0x41}, // [
    {0x02,0x04,0x08,0x10,0x20}, // backslash
    {0x41,0x41,0x7F,0x00,0x00}, // ]
    {0x04,0x02,0x01,0x02,0x04}, // ^
{0x40,0x40,0x40,0x40,0x40}, // _
    {0x00,0x01,0x02,0x04,0x00}, // `
    {0x20,0x54,0x54,0x54,0x78}, // a
    {0x7F,0x48,0x44,0x44,0x38}, // b
    {0x38,0x44,0x44,0x44,0x20}, // c
    {0x38,0x44,0x44,0x48,0x7F}, // d
    {0x38,0x54,0x54,0x54,0x18}, // e
    {0x08,0x7E,0x09,0x01,0x02}, // f
    {0x0C,0x52,0x52,0x52,0x3E}, // g
    {0x7F,0x08,0x04,0x04,0x78}, // h
    {0x00,0x44,0x7D,0x40,0x00}, // i
    {0x20,0x40,0x44,0x3D,0x00}, // j
    {0x7F,0x10,0x28,0x44,0x00}, // k
    {0x00,0x41,0x7F,0x40,0x00}, // l
    {0x7C,0x04,0x18,0x04,0x78}, // m
    {0x7C,0x08,0x04,0x04,0x78}, // n
    {0x38,0x44,0x44,0x44,0x38}, // o
    {0x7C,0x14,0x14,0x14,0x08}, // p
    {0x08,0x14,0x14,0x18,0x7C}, // q
    {0x7C,0x08,0x04,0x04,0x08}, // r
    {0x48,0x54,0x54,0x54,0x20}, // s
    {0x04,0x3F,0x44,0x40,0x20}, // t
    {0x3C,0x40,0x40,0x20,0x7C}, // u
    {0x1C,0x20,0x40,0x20,0x1C}, // v
    {0x3C,0x40,0x30,0x40,0x3C}, // w
    {0x44,0x28,0x10,0x28,0x44}, // x
    {0x0C,0x50,0x50,0x50,0x3C}, // y
    {0x44,0x64,0x54,0x4C,0x44}  // z
 };
void OLED_WriteChar(char c) {
	uint8_t so[6];
	uint8_t r;
	if( c < 0x20 || c > 0x7A) c= ' ';
	r = c - 0x20;
	memcpy(so , chuso[r], 5);
	so[5] = 0x00;
	OLED_WriteDataBuf(so,6);
}
void Oled_WriteString(uint8_t y , uint8_t x , char *cc){
	OLED_TOADO(y,x);
	while(*cc){
		OLED_WriteChar(*cc);
		cc++;
	}
}

void GPIO_Config(void){
	GPIO_InitTypeDef gp;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO ,ENABLE);
	
	gp.GPIO_Mode = GPIO_Mode_AF_PP;
	gp.GPIO_Pin = GPIO_Pin_9;
	gp.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA , &gp);
	
	gp.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gp.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA , &gp);
}

char keypad[ngang][doc] = {{'1' , '2' , '3' ,'+'},
                            {'4' , '5' ,'6' , '-'},
														{'7' ,'8' , '9' ,'x'},
														{'*' ,'0' ,'=' , '/'}
};
void delay(uint16_t time){
	uint16_t i ,j;
	for(i= 0; i< time;i++){
		for(j =0 ;j < 0x2aff ;j++);
	}
}
uint16_t a[doc] = {GPIO_Pin_4 , GPIO_Pin_5 ,GPIO_Pin_6 , GPIO_Pin_7};
uint16_t b[ngang] = {GPIO_Pin_0 , GPIO_Pin_1 , GPIO_Pin_2 , GPIO_Pin_3};
char dieukhien(){
	char key =0;
	for(uint16_t i =0 ; i< ngang ;i++){
		GPIO_ResetBits(GPIOA , b[i]);
		for(uint16_t j =0 ;j < doc ;j++){
			if(GPIO_ReadInputDataBit(GPIOA , a[j]) == RESET){
				key = keypad[i][j];
				delay(10);
				while(GPIO_ReadInputDataBit(GPIOA , a[j]) == RESET);
				delay(10);
		}
			
	}
		GPIO_SetBits(GPIOA , b[i]);
}
	return key;
}
void Oled_ClearLine(uint8_t y){
    OLED_TOADO(y, 0);
    for(uint8_t i = 0; i < 21; i++){
        OLED_WriteChar(' ');
    }
}
int main(void){
		GPIO_Config();
	I2C_Config();
	OLED_Init();
	OLED_Clear();
		Config();
		UART_Config();
				for(int i= 0 ;i< doc ;i++){
				GPIO_SetBits(GPIOA , b[i]);
			}
		
  char so_thunhat[8] = {0};
char dau[2] ={0};
char so_thuhai[8] = {0};
char bang[2] = {0};
char ketqua[8] = {0};
    int dem1 = 0 , dem2 =0; 
int so1 =0 ,so2 =0;
char toantu = 0;
uint8_t dem =0;
int check =1;
		while(1){
char phim = dieukhien();

			if(phim != 0){
				if(phim == '*'){
				dem1 =0 ;dem2 =0;
				so1 =0 ; so2 =0;
				toantu =0;
				dem =0;
					memset(so_thunhat , 0 , sizeof(so_thunhat));
					memset(so_thuhai , 0 ,sizeof(so_thuhai));
					memset(dau , 0 , sizeof(dau));
					memset(bang , 0 ,sizeof(bang));
					memset(ketqua , 0 ,sizeof(ketqua));
					
					Oled_ClearLine(4);
				}
				else if( phim == '+' || phim =='-' || phim == 'x' || phim == '/' ){
						toantu =phim;
					dem =1;
						dau[0] =toantu;
						dau[1] = '\0';
					}
				else if(phim>='0' && phim <= '9'){
					if(dem ==0){
						so1 =so1 * 10 + (phim-'0');
						if(dem1 < 7){
							so_thunhat [dem1] = phim;
							dem1 ++;
							so_thunhat[dem1] = '\0';
						}
					}
					else{
						so2 =so2*10 + (phim - '0');
						if(dem2 <7){
							so_thuhai [dem2] = phim ;
							dem2++;
							so_thuhai [dem2] = '\0';
						}
				}
		}
				else if(phim == '='){
					bang[0] ='=';
					bang[1] = '\0';
					int kq = 0;
					if(toantu =='+') kq =so1 + so2;
					if (toantu =='-') kq = so1 -so2;
					if(toantu =='x')  kq = so1 *so2;
					if (toantu == '/'){
						if(so1  <  so2 || so2 == 0) {
							Oled_WriteString(4, 55, "Loi");
							check =0 ;
						}
						else{
							kq = so1/so2;
						}
					}
						sprintf(ketqua , "%d", kq);
				
				}
				}

            OLED_TOADO(4, 2);
            OLED_WriteChar(' '); 

            Oled_WriteString(4, 2,  so_thunhat);
            Oled_WriteString(4, 20, dau);
            Oled_WriteString(4, 30, so_thuhai);
            Oled_WriteString(4, 45, bang);
				if(check ==1)
            Oled_WriteString(4, 55, ketqua);
				
	}
}
