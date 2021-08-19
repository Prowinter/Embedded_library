#ifndef __SPI_H__
#define __SPI_H__
#include "stm32f407xx.h"
#include "main.h"

#define DLY_TIM_Handle  htim7
extern TIM_HandleTypeDef DLY_TIM_Handle;

#define BIT_MODE MSB_MODE
#define MSB_MODE (0x80>>i)
#define LSB_MODE (0x01<<i)

#define SPI_SCLK_SET() HAL_GPIO_WritePin(SPI_SCLK_GPIO_Port, SPI_SCLK_Pin, GPIO_PIN_SET)
#define SPI_SCLK_RESET() HAL_GPIO_WritePin(SPI_SCLK_GPIO_Port, SPI_SCLK_Pin, GPIO_PIN_RESET)

#define SPI_MOSI_SET() HAL_GPIO_WritePin(SPI_MOSI_GPIO_Port, SPI_MOSI_Pin, GPIO_PIN_SET)
#define SPI_MOSI_RESET() HAL_GPIO_WritePin(SPI_MOSI_GPIO_Port, SPI_MOSI_Pin, GPIO_PIN_RESET)

#define SPI_MISO_READ() HAL_GPIO_ReadPin(SPI_MISO_GPIO_Port,SPI_MISO_Pin)

#define NRF_CS_SET() HAL_GPIO_WritePin(NRF_CS_GPIO_Port,NRF_CS_Pin,GPIO_PIN_SET)
#define NRF_CS_RESET() HAL_GPIO_WritePin(NRF_CS_GPIO_Port,NRF_CS_Pin,GPIO_PIN_RESET)

#define NRF_CE_SET() HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_SET)
#define NRF_CE_RESET() HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_RESET)

#define NRF_IRQ_READ() HAL_GPIO_ReadPin(NRF_IRQ_GPIO_Port,NRF_IRQ_Pin)

void delay_us(unsigned short int nus);
uint8_t SOFT_SPI_RW_MODE0(uint8_t write_data);
uint8_t SOFT_SPI_RW_MODE1(uint8_t write_data);
uint8_t SOFT_SPI_RW_MODE2(uint8_t write_data);
uint8_t SOFT_SPI_RW_MODE3(uint8_t write_data);
uint8_t NRf24L01_Write_READ_Data(uint8_t addr,uint8_t write_data);

#endif