#include "stm32f4xx_hal.h"	
#include "Mahony.h"		
#include "math.h"

/* 姿态解算，未融合磁力计数据 */
/* math.h 中的三角函数求反可替换为匿名中查表法，提高效率 */

static float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
static float exInt = 0, eyInt = 0, ezInt = 0;
extern float Yaw,Roll,Pitch;

/* Carmack算法，可实现求平方根的倒数，算法效率大于1/sqrt(x)。*/
float invSqrt(float number)
{
    volatile long i;
    volatile float x, y;
    volatile const float f = 1.5F;

    x = number * 0.5F;
    y = number;
    i = * (( long * ) &y);
    i = 0x5f3759df - ( i >> 1 );
    y = * (( float * ) &i);
    y = y * ( f - ( x * y * y ) );
    return y;
}

/*Mahony滤波算法，算法中陀螺仪数据需要转换为为弧度每秒，加速度数据因经过归一化可不进行处理*/
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az)
{
    
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;

    if(ax*ay*az==0) return;  //跳过都为0的状态
    
    gx *= 0.001064f;   //此行为陀螺仪原始数据转弧度每秒， 0.001064f = 加速度分辨率的倒数 * 角度转弧度转换率(0,017453292519943f)
    gy *= 0.001064f;   //此行为陀螺仪原始数据转弧度每秒， 0.001064f = 加速度分辨率的倒数 * 角度转弧度转换率(0,017453292519943f)
    gz *= 0.001064f;   //此行为陀螺仪原始数据转弧度每秒， 0.001064f = 加速度分辨率的倒数 * 角度转弧度转换率(0,017453292519943f)
    
    /* 加速度归一化处理 */
    norm = invSqrt(ax*ax + ay*ay + az*az);
    ax *= norm;
    ay *= norm;
    az *= norm;

    /* 地理坐标系转机体坐标系 */
    vx = 2*(q1*q3-q0*q2);
    vy = 2*((q0*q1) + (q2*q3));
    vz = q0*q0 - q1*q1 -q2*q2 + q3*q3;
    
    /* 叉乘求误差 */
    ex = (ay*vz - az*vy);
    ey = (az*vx - ax*vz);
    ez = (ax*vy - ay*vx);

    /* 叉乘误差做PI修正陀螺零偏 */
    exInt = exInt + ex*Ki;
    eyInt = eyInt + ey*Ki;
    ezInt = ezInt + ez*Ki;

    gx = gx + Kp*ex + exInt;
    gy = gy + Kp*ey + eyInt;
    gz = gz + Kp*ez + ezInt;

    /* 一阶龙格库塔法解微分方程 */
    q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
    q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
    q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
    q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;

    /* 归一化 */
    norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    q0 *= norm;
    q1 *= norm;
    q2 *= norm;
    q3 *= norm;

    /* 转姿态角 */
    Yaw = atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2 * q2 - 2 * q3 * q3 + 1) * 57.3; // yaw
    Pitch  = asin(-2 * q1 * q3 + 2 * q0 * q2)* 57.3; // pitch
    Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1) * 57.3; // roll
}

