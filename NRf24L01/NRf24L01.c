#include "NRf24L01.h"

const uint8_t TxAddr_data[5]={0x11,0x22,0x33,0x44,0x66};
const uint8_t RxAddr_data[5]={0x11,0x22,0x33,0x44,0x66};

void delay_us(unsigned short int nus)
{
    __HAL_TIM_SetCounter(&DLY_TIM_Handle,0);
    HAL_TIM_Base_Start(&DLY_TIM_Handle);
    while(__HAL_TIM_GetCounter(&DLY_TIM_Handle)<nus);
    HAL_TIM_Base_Stop(&DLY_TIM_Handle);
}

/*
ģʽ0��CPOL=0��CPHA =0  SCK����Ϊ�͵�ƽ��������SCK�������ر�����(��ȡ����)

ģʽ1��CPOL=0��CPHA =1  SCK����Ϊ�͵�ƽ��������SCK���½��ر�����(��ȡ����)

ģʽ2��CPOL=1��CPHA =0  SCK����Ϊ�ߵ�ƽ��������SCK���½��ر�����(��ȡ����)

ģʽ3��CPOL=1��CPHA =1  SCK����Ϊ�ߵ�ƽ��������SCK�������ر�����(��ȡ����)
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

uint8_t NRf24L01_WriteBuf(uint8_t addr, uint8_t *pBuf, uint8_t len)
{
	uint8_t status;    
	NRF_CS_RESET();
	status = SOFT_SPI_RW_MODE0(addr);
	for(uint8_t i=0; i<len; i++)	
    {
		SOFT_SPI_RW_MODE0(pBuf[i]);		
	}
	NRF_CS_SET();
	return status;
}
 
uint8_t NRf24L01_ReadBuf(uint8_t addr, uint8_t *pBuf, uint8_t len)
{
	uint8_t status;	
	NRF_CS_RESET();
	status = SOFT_SPI_RW_MODE0(addr);
	for(uint8_t i = 0; i<len ;i++)
    {
		pBuf[i] = SOFT_SPI_RW_MODE0(0xFF);		
	}
	NRF_CS_SET();
	return status;
}

void NRf24L01_Init(void)
{
    NRF_CE_RESET();
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_RF_CH,110);                     //  ������Ƶͨ��
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_SETUP_AW,0x03);                //  ���õ�ַ����
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_RF_SETUP,0x06);                //  1Mbps,0dBm,No Lna
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_SETUP_RETR,0x0f);              //  �����ش���ʱ���ش�����250us,15
    NRf24L01_Write_READ_Data(NRF_FLUSH_TX,0xff);                                //  ���TX_FIFO����
    NRf24L01_Write_READ_Data(NRF_FLUSH_RX,0xff);                                //  ���RX_FIFO����
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_STATUS,0x7E);                  //  ���״̬�Ĵ�������
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_RX_PW_P0,32);                  //  ѡ��ͨ��0����Ч���ݿ��
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_EN_AA,0x01);                   //  ʹ��ͨ��0���Զ�Ӧ��
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_CONFIG,0x02);                  //  �л���Start Upģʽ

    // NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_SETUP_RETR,0x08);              //  �����ط�
    HAL_Delay(2);
}

void Rx_Mode(void)
{
    NRF_CE_RESET();
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_CONFIG,0x0f);    //PWR_UP,PRIM_RXʹ�� 
    NRf24L01_WriteBuf(NRF_WRITE_REG + NRF_RX_ADDR_P0,(uint8_t *)TxAddr_data,5); 
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_EN_RXADDR,0x01);
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_EN_AA,0x01);
    NRF_CE_SET();
}

void Tx_Mode(void)
{
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_CONFIG,0x0E);    //PWR_UP,PRIM_RXʹ��
    NRf24L01_WriteBuf(NRF_WRITE_REG + NRF_TX_ADDR,(uint8_t *)TxAddr_data,5);    
    NRf24L01_WriteBuf(NRF_WRITE_REG + NRF_RX_ADDR_P0,(uint8_t *)TxAddr_data,5); 
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_EN_AA,0x01);
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_CONFIG,0x0E);
}

uint8_t NRf24L01_TxPacket(uint8_t *txbuf,uint8_t len)
{
    uint8_t status;
    NRF_CE_RESET();
    NRf24L01_WriteBuf(NRF_WR_TX_PLOAD,txbuf,len);   // д���ݵ�TX_BUFF    
    // NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_CONFIG,0x0E);	             // ����Ϊ����ģʽ,���������ж�	    
    NRF_CE_SET();
    while(NRF_IRQ_READ());
    NRf24L01_Write_READ_Data(NRF_FLUSH_TX,0xff); 
    status = NRf24L01_Write_READ_Data(NRF_READ_REG + NRF_STATUS,0xff);
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_STATUS,status);
    if(status&NRF_MAX_RT)
    {
        return NRF_MAX_RT;
    }
    else if(status&NRF_TX_DS)
    {
        return NRF_TX_DS;
    }
    return 0xff;
}

uint8_t NRf24L01_RePacket(uint8_t *txbuf,uint8_t len)
{
    uint8_t status;
    status = NRf24L01_Write_READ_Data(NRF_READ_REG + NRF_STATUS,0xff);
    NRf24L01_Write_READ_Data(NRF_WRITE_REG + NRF_STATUS,status);
    if(status&NRF_RX_DR)
    {
        NRf24L01_ReadBuf(NRF_RD_RX_PLOAD,txbuf,len);
        return NRF_RX_DR;   //���ճɹ�
    }
    return 0xff;    //����ԭ��ʧ��

}

// �Լ캯�� �Լ�ͨ������1���Լ첻ͨ������0.
uint8_t NRf24L01_Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i;
	NRf24L01_WriteBuf(NRF_WRITE_REG + NRF_TX_ADDR,buf,5);
	NRf24L01_ReadBuf(NRF_READ_REG + NRF_TX_ADDR,buf,5);
	for(i=0; i<5; i++)if(buf[i]!=0xA5)break;
	if(i!=5)return 0;
	return 1;
}