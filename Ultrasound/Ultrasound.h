#ifndef __ULTRASOUND_H
#define __ULTRASOUND_H

#include "main.h"

#define Max_Count 2000 //×î´óÌ½²â¾àÀë34cm 

#define DLY_TIM_Handle  htim4
extern TIM_HandleTypeDef DLY_TIM_Handle;

void delay_us(unsigned short int nus);

unsigned short int Get_Ultrasound_Value(GPIO_TypeDef * GPIO_Port,uint16_t GPIO_PIN);

void Shoot_Ultrasound(GPIO_TypeDef * GPIO_Port,uint16_t GPIO_PIN);

float Value_To_CM(unsigned short int Value);

#endif

