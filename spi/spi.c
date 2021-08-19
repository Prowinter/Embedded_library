#include "SPI.h"

void delay_us(unsigned short int nus)
{
    __HAL_TIM_SetCounter(&DLY_TIM_Handle,0);
    HAL_TIM_Base_Start(&DLY_TIM_Handle);
    while(__HAL_TIM_GetCounter(&DLY_TIM_Handle)<nus);
    HAL_TIM_Base_Stop(&DLY_TIM_Handle);
}

/*
模式0：CPOL=0，CPHA =0  SCK空闲为低电平，数据在SCK的上升沿被采样(提取数据)

模式1：CPOL=0，CPHA =1  SCK空闲为低电平，数据在SCK的下降沿被采样(提取数据)

模式2：CPOL=1，CPHA =0  SCK空闲为高电平，数据在SCK的下降沿被采样(提取数据)

模式3：CPOL=1，CPHA =1  SCK空闲为高电平，数据在SCK的上升沿被采样(提取数据)
*/
uint8_t SOFT_SPI_RW_MODE0(uint8_t write_data)
{
    uint8_t i,read_data=0;
    SPI_SCLK_RESET();
    delay_us(1);
    for(i=0;i<8;i++)
    {
        (write_data&BIT_MODE)?SPI_MOSI_SET():SPI_MOSI_RESET();
        delay_us(1);
        SPI_SCLK_SET();
        delay_us(1);
        if(SPI_MISO_READ()) read_data|=BIT_MODE;
        delay_us(1);
        SPI_SCLK_RESET();
        delay_us(1);
    }
    return read_data;
}

uint8_t SOFT_SPI_RW_MODE1(uint8_t write_data)
{
    uint8_t i,read_data=0;
    SPI_SCLK_RESET();
    delay_us(1);
    for(i=0;i<8;i++)
    {
        SPI_SCLK_SET();
        delay_us(1);
        (write_data&BIT_MODE)?SPI_MOSI_SET():SPI_MOSI_RESET();
        delay_us(1);
        SPI_SCLK_RESET();
        delay_us(1);
        if(SPI_MISO_READ()) read_data|=BIT_MODE;
        delay_us(1);
    }
    return read_data;
}

uint8_t SOFT_SPI_RW_MODE2(uint8_t write_data)
{
    uint8_t i,read_data=0;
    SPI_SCLK_SET();
    delay_us(1);
    for(i=0;i<8;i++)
    {
        (write_data&BIT_MODE)?SPI_MOSI_SET():SPI_MOSI_RESET();
        delay_us(1);
        SPI_SCLK_RESET();
        delay_us(1);
        if(SPI_MISO_READ()) read_data|=BIT_MODE;
        delay_us(1);
        SPI_SCLK_SET();
        delay_us(1);
    }
    return read_data;
}

uint8_t SOFT_SPI_RW_MODE3(uint8_t write_data)
{
    uint8_t i,read_data=0;
    SPI_SCLK_SET();
    delay_us(1);
    for(i=0;i<8;i++)
    {
        SPI_SCLK_RESET();
        delay_us(1);
        (write_data&BIT_MODE)?SPI_MOSI_SET():SPI_MOSI_RESET();
        delay_us(1);
        SPI_SCLK_SET();
        delay_us(1);
        if(SPI_MISO_READ()) read_data|=BIT_MODE;
        delay_us(1);
    }
    return read_data;
}

uint8_t NRf24L01_Write_READ_Data(uint8_t addr,uint8_t write_data)
{
    uint8_t read_data;
    NRF_CS_RESET();
    SOFT_SPI_RW_MODE0(addr);
    read_data = SOFT_SPI_RW_MODE0(write_data);
    NRF_CS_SET();
    return read_data;
}