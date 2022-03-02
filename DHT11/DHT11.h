#ifndef __DHT11_H__
#define __DHT11_H__
#include "main.h"

#define DLY_TIM_Handle  htim2
extern TIM_HandleTypeDef DLY_TIM_Handle;
#define Bit_RESET 0;
#define Bit_SET 1;


#define GPIOA_MODER_L            *(unsigned int*)(GPIOA_BASE+0x00)
#define GPIOA_MODER_H            *(unsigned int*)(GPIOA_BASE+0x04)

#define DHT11_DAT_GPIOx GPIOA
#define DHT11_DAT_Pin_Num 0
#define DHT11_DAT_GPIO_Pin GPIO_PIN_0
#define Get_DAT_Pin_Num (DHT11_DAT_Pin_Num>7?DHT11_DAT_Pin_Num-8:DHT11_DAT_Pin_Num)

#define DAT_IN()	{GPIOA_MODER_L&=0XFFFFFFF0;GPIOA_MODER_L|=8<<(4*Get_DAT_Pin_Num);}
#define DAT_OUT()	{GPIOA_MODER_L&=0XFFFFFFF0;GPIOA_MODER_L|=3<<(4*Get_DAT_Pin_Num);}

typedef struct
{
	unsigned char humi_int;		//湿度的整数部分
	unsigned char humi_deci;	//湿度的小数部分
	unsigned char temp_int;	 	//温度的整数部分
	unsigned char temp_deci;	//温度的小数部分
	unsigned char check_sum;	//校验和
		                 
}DHT11_Data_TypeDef;

void delay_us(unsigned short int nus);
void DHT11_Start(void);
void DHT11_Stop(void);
uint8_t Read_Byte(void);
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);

#endif

