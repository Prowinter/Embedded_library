//////////////////////////////////////////////////////////////////////////////////	 
//I2C——HAL库初始化程序     By:Prowinter
//us级延时采用TIM定时提供 
//内含寄存器操作,具体修改请参考数据手册
//不会修改请注释相关内容转换为Hal库操作
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

/* 寄存器操作 */
#define SDA_IN()	{GPIOE_MODER_H&=0X0F0FFFFF;GPIOE_MODER_H|=8<<(4*Get_I2C_Pin_Num);}
#define SDA_OUT()	{GPIOE_MODER_H&=0X0F0FFFFF;GPIOE_MODER_H|=3<<(4*Get_I2C_Pin_Num);}


#define I2C_SCLK_SET() HAL_GPIO_WritePin(I2C_SCLK_GPIOx, I2C_SCLK_GPIO_Pin, GPIO_PIN_SET)
#define I2C_SCLK_RESET() HAL_GPIO_WritePin(I2C_SCLK_GPIOx, I2C_SCLK_GPIO_Pin, GPIO_PIN_RESET)

#define I2C_SDA_SET() HAL_GPIO_WritePin(I2C_SDA_GPIOx, I2C_SDA_GPIO_Pin, GPIO_PIN_SET)
#define I2C_SDA_RESET() HAL_GPIO_WritePin(I2C_SDA_GPIOx, I2C_SDA_GPIO_Pin, GPIO_PIN_RESET)
#define I2C_SDA_READ() HAL_GPIO_ReadPin(I2C_SDA_GPIOx, I2C_SDA_GPIO_Pin)

//IO方向设置
//IO操作函数	

//IIC所有操作函数
/* Hal库操作*/
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
















