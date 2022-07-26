#include "stdio.h"

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
    // HAL库函数版本
    // HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1 , 0xffff);
    // return ch;

    // 寄存器版本
    while ((USART1->SR & 0X40) == 0);
    USART1->DR = (uint8_t) ch;
    return ch;
}

// VScode 需额外添加此函数
// 若需使用Printf,Sprintf操作Float型变量，还需在Makefile 对进行此操作 LDFLAGS = $(MCU) -u_printf_float -u_sprintf_float
// 若使用CMakelists构建工程 还需要将 --specs=nano.specs 替换为 --specs=nosys.specs
int _write(int file, char *ptr,int len)
{
  int DataIdx;
  for (DataIdx=0;DataIdx<len;DataIdx++)
  {
    __io_putchar(*ptr++);
  }
  return len;
}
