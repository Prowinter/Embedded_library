#include "Ultrasound.h"


/**
  * @brief  Delay us
  *
  * @note   This function uses clock to delay specific us
  *
  * @param  nus:this is the number you want specific
  * 
  * @retval None
  */
void delay_us(unsigned short int nus)
{
		__HAL_TIM_SetCounter(&DLY_TIM_Handle,0);
    HAL_TIM_Base_Start(&DLY_TIM_Handle);
    while(__HAL_TIM_GetCounter(&DLY_TIM_Handle)<nus);
    HAL_TIM_Base_Stop(&DLY_TIM_Handle);
}


/**
  * @brief  Send_Ultrasound
  *
  * @param  GPIO_Port:where x can be (A..G depending on device used) to select the GPIO peripheral
  * @param  GPIO_PIN: specifies the port bit to be written.
  *          					This parameter can be one of GPIO_PIN_x where x can be (0..15).
  * @retval None
  */
void Shoot_Ultrasound(GPIO_TypeDef * GPIO_Port,uint16_t GPIO_PIN)
{
	HAL_GPIO_WritePin(GPIO_Port,GPIO_PIN,GPIO_PIN_RESET);
	delay_us(10);
	HAL_GPIO_WritePin(GPIO_Port,GPIO_PIN,GPIO_PIN_SET);
	delay_us(20);
	HAL_GPIO_WritePin(GPIO_Port,GPIO_PIN,GPIO_PIN_RESET);
}


/**
  * @brief  Get ultrasound clock count
  *
  * @note   This function uses 1us clock to count high level times
  *
  * @param  GPIO_Port:where x can be (A..G depending on device used) to select the GPIO peripheral
  * @param  GPIO_PIN: specifies the port bit to be written.
  *          					This parameter can be one of GPIO_PIN_x where x can be (0..15).
  * 
  * @retval None
  */
unsigned short int Get_Ultrasound_Value(GPIO_TypeDef * GPIO_Port,uint16_t GPIO_PIN)
{
	unsigned short int count = 0;
	while(!HAL_GPIO_ReadPin(GPIO_Port,GPIO_PIN))
	{
		delay_us(100);
		count ++;
		if (count > 5000)
		{
			return Max_Count;
		}
	}
	__HAL_TIM_SetCounter(&DLY_TIM_Handle,0);
	HAL_TIM_Base_Start(&DLY_TIM_Handle);
	while(HAL_GPIO_ReadPin(GPIO_Port,GPIO_PIN));
	HAL_TIM_Base_Stop(&DLY_TIM_Handle);
	return __HAL_TIM_GetCounter(&DLY_TIM_Handle);
}

/**
  * @brief  ultrasound clock count to cm
  *
  * @note   This function is convert ultrasound clock count to cm
  *
  * @param  Value:ultrasound clock count
  * 
  * @retval CM
  */
float Value_To_CM(unsigned short int Value)
{
	return ((float)(Value * 17))/1000;
}