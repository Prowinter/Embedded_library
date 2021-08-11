#include "stm32f4xx_hal.h"	
#include "Mahony.h"		
#include "math.h"

/* ��̬���㣬δ�ںϴ��������� */
/* math.h �е����Ǻ����󷴿��滻Ϊ�����в�������Ч�� */

static float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
static float exInt = 0, eyInt = 0, ezInt = 0;
extern float Yaw,Roll,Pitch;

/* Carmack�㷨����ʵ����ƽ�����ĵ������㷨Ч�ʴ���1/sqrt(x)��*/
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

/*Mahony�˲��㷨���㷨��������������Ҫת��ΪΪ����ÿ�룬���ٶ������򾭹���һ���ɲ����д���*/
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az)
{
    
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;

    if(ax*ay*az==0) return;  //������Ϊ0��״̬
    
    gx *= 0.001064f;   //����Ϊ������ԭʼ����ת����ÿ�룬 0.001064f = ���ٶȷֱ��ʵĵ��� * �Ƕ�ת����ת����(0,017453292519943f)
    gy *= 0.001064f;   //����Ϊ������ԭʼ����ת����ÿ�룬 0.001064f = ���ٶȷֱ��ʵĵ��� * �Ƕ�ת����ת����(0,017453292519943f)
    gz *= 0.001064f;   //����Ϊ������ԭʼ����ת����ÿ�룬 0.001064f = ���ٶȷֱ��ʵĵ��� * �Ƕ�ת����ת����(0,017453292519943f)
    
    /* ���ٶȹ�һ������ */
    norm = invSqrt(ax*ax + ay*ay + az*az);
    ax *= norm;
    ay *= norm;
    az *= norm;

    /* ��������ϵת��������ϵ */
    vx = 2*(q1*q3-q0*q2);
    vy = 2*((q0*q1) + (q2*q3));
    vz = q0*q0 - q1*q1 -q2*q2 + q3*q3;
    
    /* �������� */
    ex = (ay*vz - az*vy);
    ey = (az*vx - ax*vz);
    ez = (ax*vy - ay*vx);

    /* ��������PI����������ƫ */
    exInt = exInt + ex*Ki;
    eyInt = eyInt + ey*Ki;
    ezInt = ezInt + ez*Ki;

    gx = gx + Kp*ex + exInt;
    gy = gy + Kp*ey + eyInt;
    gz = gz + Kp*ez + ezInt;

    /* һ�������������΢�ַ��� */
    q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
    q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
    q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
    q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;

    /* ��һ�� */
    norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    q0 *= norm;
    q1 *= norm;
    q2 *= norm;
    q3 *= norm;

    /* ת��̬�� */
    Yaw = atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2 * q2 - 2 * q3 * q3 + 1) * 57.3; // yaw
    Pitch  = asin(-2 * q1 * q3 + 2 * q0 * q2)* 57.3; // pitch
    Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1) * 57.3; // roll
}

