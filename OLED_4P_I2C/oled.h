#ifndef __OLED_H__
#define __OLED_H__

#include "myiic.h"

void OLED_Init(void);
void OLED_ON(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_CLS(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
void test_bmp(void);
void OLED_ShowInt(unsigned char x, unsigned char y,int tem,unsigned char TextSize);
void OLED_ShowFloat(unsigned char x, unsigned char y,float tem,unsigned char TextSize);\
void WriteCmd(unsigned char command);
void WriteDat(unsigned char data);
#endif

