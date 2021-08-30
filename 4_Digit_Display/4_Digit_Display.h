//////////////////////////////////////////////////////////////////////////////////	 
//I2C����HAL���ʼ������     By:Prowinter
//us����ʱ����TIM��ʱ�ṩ 
//�ں��Ĵ�������,�����޸���ο������ֲ�
//�����޸���ע���������ת��ΪHal�����
////////////////////////////////////////////////////////////////////////////////// 	
#ifndef __4_DIGIT_DISPLAY_H__
#define __4_DIGIT_DISPLAY_H__
#include "stm32f4xx_hal.h"

#define DLY_TIM_Handle  htim7
extern TIM_HandleTypeDef DLY_TIM_Handle;
#define GPIOE_MODER_L            *(unsigned int*)(GPIOE_BASE+0x00)
#define GPIOE_MODER_H            *(unsigned int*)(GPIOE_BASE+0x04)
	
#define I2C_SCLK_GPIOx GPIOE
#define I2C_SCLK_GPIO_Pin GPIO_PIN_0

#define I2C_SDA_GPIOx GPIOE
#define I2C_SDA_Pin_Num 1
#define I2C_SDA_GPIO_Pin GPIO_PIN_1
#define Get_I2C_Pin_Num (I2C_SDA_Pin_Num>7?I2C_SDA_Pin_Num-8:I2C_SDA_Pin_Num)

/* �Ĵ������� */
#define SDA_IN()	{GPIOE_MODER_H&=0X0F0FFFFF;GPIOE_MODER_H|=8<<(4*Get_I2C_Pin_Num);}
#define SDA_OUT()	{GPIOE_MODER_H&=0X0F0FFFFF;GPIOE_MODER_H|=3<<(4*Get_I2C_Pin_Num);}


#define I2C_SCLK_SET() HAL_GPIO_WritePin(I2C_SCLK_GPIOx, I2C_SCLK_GPIO_Pin, GPIO_PIN_SET)
#define I2C_SCLK_RESET() HAL_GPIO_WritePin(I2C_SCLK_GPIOx, I2C_SCLK_GPIO_Pin, GPIO_PIN_RESET)

#define I2C_SDA_SET() HAL_GPIO_WritePin(I2C_SDA_GPIOx, I2C_SDA_GPIO_Pin, GPIO_PIN_SET)
#define I2C_SDA_RESET() HAL_GPIO_WritePin(I2C_SDA_GPIOx, I2C_SDA_GPIO_Pin, GPIO_PIN_RESET)
#define I2C_SDA_READ() HAL_GPIO_ReadPin(I2C_SDA_GPIOx, I2C_SDA_GPIO_Pin)

//IO��������
//IO��������	

//IIC���в�������
/* Hal�����*/
//void SDA_IN(void);
//void SDA_OUT(void);

//************definitions for TM1637*********************
#define ADDR_AUTO  0x40
#define ADDR_FIXED 0x44
#define STARTADDR  0xc0 


void delay_us(unsigned short int nus);
void TM1637_start(void);
void TM1637_stop(void);
void TM1637_ACK(void);
void TM1637_Writ_Byt(unsigned char Byt);
void TM1637_Inc(unsigned char data[],unsigned char brightness);
void TM1637_Cle(void);
void TM1637_Noinc(unsigned char addr,unsigned char data,unsigned char brightness);
#endif
















