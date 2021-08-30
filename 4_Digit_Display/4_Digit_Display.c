#include "4_Digit_Display.h"


unsigned char tab[] =
{
    0x3F,/*0*/
    0x06,/*1*/
    0x5B,/*2*/
    0x4F,/*3*/
    0x66,/*4*/
    0x6D,/*5*/
    0x7D,/*6*/
    0x07,/*7*/
    0x7F,/*8*/
    0x6F,/*9*/
    0x77,/*10 A*/
    0x7C,/*11 b*/
    0x58,/*12 c*/
    0x5E,/*13 d*/
    0x79,/*14 E*/
    0x71,/*15 F*/
    0x76,/*16 H*/
    0x38,/*17 L*/
    0x54,/*18 n*/
    0x73,/*19 P*/
    0x3E,/*20 U*/
    0x00,/*21 黑屏*/
};

unsigned char brightness_data[] = 
{
	0x80,/*1/16*/
	0x81,/*2/16*/
	0x82,/*4/16*/
	0x83,/*10/16*/
	0x84,/*11*16*/
	0x85,/*12*16*/
	0x86,/*13/16*/
	0x87,/*14/16*/
};

//void SDA_IN(void)
//{
//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//  GPIO_InitStruct.Pin = GPIO_PIN_13;
//  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//}

//void SDA_OUT(void)
//{
//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//  GPIO_InitStruct.Pin = GPIO_PIN_13;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_PULLUP;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//}

void delay_us(unsigned short int nus)
{
		__HAL_TIM_SetCounter(&DLY_TIM_Handle,0);
    HAL_TIM_Base_Start(&DLY_TIM_Handle);
    while(__HAL_TIM_GetCounter(&DLY_TIM_Handle)<nus);
    HAL_TIM_Base_Stop(&DLY_TIM_Handle);
}


/**
  * @brief  Send a start flag to TM1637
  * @param  None
  * @retval None
  */
void TM1637_start(void)
{
 SDA_OUT();
 I2C_SCLK_SET();
 delay_us(2);
 I2C_SDA_SET();
 delay_us(2);
 I2C_SDA_RESET();
} 


/**
  * @brief  Send a stop flag to TM1637
  * @param  None
  * @retval None
  */
void TM1637_stop(void)
{
 SDA_OUT();
 I2C_SCLK_SET();
 delay_us(2);
 I2C_SDA_RESET();
 delay_us(2);
 I2C_SDA_SET();
}
 

/**
  * @brief  Wait for the TM1637 to produce an ACK answer
  * @param  None
  * @retval None
  */
void TM1637_ACK(void)
{
	SDA_IN();
	I2C_SCLK_RESET();
	delay_us(5);
	while(I2C_SDA_READ());
	delay_us(2);
	I2C_SCLK_SET();
	delay_us(2);
	I2C_SCLK_RESET();
}
 
/**
  * @brief  Send a data to TM1637
  * @param  Byt which one will send to TM1637
  *         This parameter is kept for compatibility with other series.
  * @retval None.
  */
void TM1637_Writ_Byt(unsigned char Byt)
{
	unsigned char i;
	SDA_OUT();
	for(i=0;i<8;i++)
	{
		I2C_SCLK_RESET();
		if(Byt&0x01) //低位在前
		{
			I2C_SDA_SET();
		}
		else
		{
			I2C_SDA_RESET();
		}
		delay_us(3);
		Byt>>=1;
		I2C_SCLK_SET();
		delay_us(3);
	}
}

/**
  * @brief  Display data using growing mode.
  * @param  data[] which ones will display on TM1637
  *         This parameter member can be one of the tab arrays.
  * @param  brightness Adjust the display brightness.
  *         This parameter can be one of the brightness_data arrays.You need to or the 0x08 when you want to open the display.
  * @retval None
  */
void TM1637_Inc(unsigned char data[],unsigned char brightness)
{
	unsigned char i,cnt;
	cnt = sizeof(data)/sizeof(data[0]);
	// 设置固定地址
	TM1637_start();
	TM1637_Writ_Byt(0x40);
	TM1637_ACK();
	TM1637_stop();
	
	// 设置首地址
	TM1637_start();
	TM1637_Writ_Byt(STARTADDR);
	TM1637_ACK();
	for(i=0;i<cnt;i++)
	{
	TM1637_Writ_Byt(data[i]);
	TM1637_ACK();
	}
	TM1637_stop();

	// 开启显示,设置脉冲宽度为14/16
	TM1637_start();
	TM1637_Writ_Byt(brightness);
	TM1637_ACK();
	TM1637_stop();
}


/**
  * @brief  Clear the TM1637 display.
  * @param  None
  * @retval None
  */
void TM1637_Cle(void)
{
	unsigned char i;
	// 设置固定地址
	TM1637_start();
	TM1637_Writ_Byt(ADDR_AUTO);
	TM1637_ACK();
	TM1637_stop();
	
	// 设置首地址
	TM1637_start();
	TM1637_Writ_Byt(STARTADDR);
	TM1637_ACK();
	for(i=0;i<4;i++)
	{
	TM1637_Writ_Byt(0x00);
	TM1637_ACK();
	}
	TM1637_stop();

	// 开启显示,设置脉冲宽度为14/16
	TM1637_start();
	TM1637_Writ_Byt(0x87|0x08);
	TM1637_ACK();
	TM1637_stop();
}

/**
  * @brief  Display data using no growing mode.
  * @param  addr where the data will display.
  *         This parameter can be one of [0,1,2,3]
  * @param  data which one will display on TM1637
  *         This parameter can be one of the tab arrays.
  * @param  brightness Adjust the display brightness.
  *         This parameter can be one of the brightness_data arrays.You need to or the 0x08 when you want to open the display.
  * @retval None
  */
void TM1637_Noinc(unsigned char addr,unsigned char data,unsigned char brightness)
{
	// 设置固定地址
	TM1637_start();
	TM1637_Writ_Byt(ADDR_FIXED);
	TM1637_ACK();
	TM1637_stop();
	
	// 设置首地址
	TM1637_start();
	TM1637_Writ_Byt(STARTADDR|addr);
	TM1637_ACK();
	TM1637_Writ_Byt(data);			
	TM1637_ACK();
	TM1637_stop();

	// 开启显示,设置脉冲宽度为14/16
	TM1637_start();
	TM1637_Writ_Byt(brightness);
	TM1637_ACK();
	TM1637_stop();
}