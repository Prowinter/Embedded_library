/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart2;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	
	/// 指纹模块传输协议解析
	
	extern uint8_t USART_RX_BUF[200];
	extern uint16_t USART_RX_STA;
	static uint8_t tem=0;
	uint8_t Res;	    
	if((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART2->DR;//(USART1->DR);	//读取接收到的数据
		if((USART_RX_STA&0x8000)==0) //接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0xEF
			{
				if(USART_RX_STA&0x2000)//接收到了0x01
				{
					if(USART_RX_STA&0x1000)//接收芯片地址
					{
						if(USART_RX_STA&0x0800)//接收包标识
						{
							if(USART_RX_STA&0x0400)//接收第一位包长度
							{
								if(USART_RX_STA&0x0200)//接收第二位包长度
								{
									if(USART_RX_STA&0x0100)//接收到最后一位数据
									{
										USART_RX_BUF[8+USART_RX_BUF[7]+USART_RX_BUF[8]] = Res;
										tem++;
										//和检验
										uint16_t sum = 0;
										for(int i=0;i<(1+tem);i++)
										{
											sum += USART_RX_BUF[6+i];
										}
										if(USART_RX_BUF[8]==0x19 && USART_RX_BUF[6]==0x07) sum-=6;
										//通过
										if (sum == ((USART_RX_BUF[7+tem]<<8) + USART_RX_BUF[8+tem])) {USART_RX_STA|=0x8000;}
										else USART_RX_STA=0;
									}
									else
									{
										if((USART_RX_STA-1)&0x00FF)  //USART_RX_STA=19 18 17
										{
											USART_RX_STA--;						//18 17
											tem++;
											USART_RX_BUF[8+tem] = Res; // USART_RX_BUF[7]+USART_RX_BUF[8] = 19 1
											
											if(((USART_RX_STA-1)&0x00FF) == 0) USART_RX_STA|= 0x0100; // 1   0
										}
									}
								}
								else
								{
									USART_RX_STA |= 0x0200;
									USART_RX_BUF[8] = Res; ;
									USART_RX_STA += Res; // 包长度
									tem = 0;
								}
							}
							else
							{
								USART_RX_BUF[7] = Res;
								USART_RX_STA |= 0x0400;
								USART_RX_STA += Res; // 包长度
							}

						}
						else
						{
							USART_RX_BUF[6] = Res;
							USART_RX_STA |= 0x0800;
						}
					}
					else
					{
						if(USART_RX_STA&0x00FF)
						{
							USART_RX_STA--;
							USART_RX_BUF[1+(4-USART_RX_STA&0x00FF)] = Res;
							if((USART_RX_STA&0x00FF) == 0) USART_RX_STA|= 0x1000;
						}
					}
				}
				else
				{
					if(Res == 0x01) 
					{
						USART_RX_BUF[1] = Res;
						USART_RX_STA |= 0x2000;
						USART_RX_STA += 4;		//芯片地址长度4
					}
					else USART_RX_STA = 0;
				}
			}
			else
			{
				if(Res == 0xEF) 
				{
					USART_RX_BUF[0] = Res;
					USART_RX_STA |= 0x4000;
				}
			}
		}
	}
	
	
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
	HAL_UART_Receive_IT(&huart2, (uint8_t *)USART_RX_BUF, 1);
  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */
	extern unsigned char KEY;
	if (__HAL_GPIO_EXTI_GET_IT(KEY_1_Pin) != 0x00u)
  {
		if(!HAL_GPIO_ReadPin(KEY_1_GPIO_Port,KEY_1_Pin))
		{
			if(!HAL_GPIO_ReadPin(KEY_1_GPIO_Port,KEY_1_Pin)) KEY = 0x01;
			HAL_Delay(300);
			if(!HAL_GPIO_ReadPin(KEY_1_GPIO_Port,KEY_1_Pin)) KEY = 0x02;
		}
  }
	if (__HAL_GPIO_EXTI_GET_IT(KEY_2_Pin) != 0x00u)
  {
		KEY = 0x05;
	}
	if (__HAL_GPIO_EXTI_GET_IT(KEY_3_Pin) != 0x00u)
  {
		if(!HAL_GPIO_ReadPin(KEY_3_GPIO_Port,KEY_3_Pin))
		{
			if(!HAL_GPIO_ReadPin(KEY_3_GPIO_Port,KEY_3_Pin)) KEY = 0x03;
			HAL_Delay(300);
			if(!HAL_GPIO_ReadPin(KEY_3_GPIO_Port,KEY_3_Pin)) KEY = 0x04;
		}
	}
  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
