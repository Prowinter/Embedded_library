#ifndef __MAHONY_H
#define __MAHONY_H
#include "stm32f4xx_hal.h"

#define Kp 20.0f        // Mahony算法中Kp
#define Ki 0.001f       // Mahony算法中Kp
#define halfT 0.001f    // 加速度，陀螺仪采样速度的一半

float invSqrt(float number);
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az);

#endif
