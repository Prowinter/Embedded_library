#ifndef __MAHONY_H
#define __MAHONY_H
#include "stm32f4xx_hal.h"

#define Kp 20.0f        // Mahony�㷨��Kp
#define Ki 0.001f       // Mahony�㷨��Kp
#define halfT 0.001f    // ���ٶȣ������ǲ����ٶȵ�һ��

float invSqrt(float number);
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az);

#endif
