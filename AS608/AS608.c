#include "AS608.h"
extern uint8_t USART_RX_BUF[200];
extern uint16_t USART_RX_STA;

//AS608 Э�����
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	
	/// ָ��ģ�鴫��Э�����
	
	extern uint8_t USART_RX_BUF[200];
	extern uint16_t USART_RX_STA;
	static uint8_t tem=0;
	uint8_t Res;	    
	if((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_RXNE)!=RESET))  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res = USART2->DR;//(USART1->DR);	//��ȡ���յ�������
		if((USART_RX_STA&0x8000)==0) //����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0xEF
			{
				if(USART_RX_STA&0x2000)//���յ���0x01
				{
					if(USART_RX_STA&0x1000)//����оƬ��ַ
					{
						if(USART_RX_STA&0x0800)//���հ���ʶ
						{
							if(USART_RX_STA&0x0400)//���յ�һλ������
							{
								if(USART_RX_STA&0x0200)//���յڶ�λ������
								{
									if(USART_RX_STA&0x0100)//���յ����һλ����
									{
										USART_RX_BUF[8+USART_RX_BUF[7]+USART_RX_BUF[8]] = Res;
										tem++;
										//�ͼ���
										uint16_t sum = 0;
										for(int i=0;i<(1+tem);i++)
										{
											sum += USART_RX_BUF[6+i];
										}
										if(USART_RX_BUF[8]==0x19 && USART_RX_BUF[6]==0x07) sum-=6;
										//ͨ��
										if (sum == ((USART_RX_BUF[7+tem]<<8) + USART_RX_BUF[8+tem])) {USART_RX_STA|=0x8000;}
										else USART_RX_STA=0;
									}
									else
									{
										if((USART_RX_STA-1)&0x00FF)  //USART_RX_STA=19 18 17
										{
											USART_RX_STA--;						//18 17
											tem++;
											USART_RX_BUF[8+tem] = Res; // USART_RX_BUF[7]+USART_RX_BUF[8] = 19 1
											
											if(((USART_RX_STA-1)&0x00FF) == 0) USART_RX_STA|= 0x0100; // 1   0
										}
									}
								}
								else
								{
									USART_RX_STA |= 0x0200;
									USART_RX_BUF[8] = Res; ;
									USART_RX_STA += Res; // ������
									tem = 0;
								}
							}
							else
							{
								USART_RX_BUF[7] = Res;
								USART_RX_STA |= 0x0400;
								USART_RX_STA += Res; // ������
							}

						}
						else
						{
							USART_RX_BUF[6] = Res;
							USART_RX_STA |= 0x0800;
						}
					}
					else
					{
						if(USART_RX_STA&0x00FF)
						{
							USART_RX_STA--;
							USART_RX_BUF[1+(4-USART_RX_STA&0x00FF)] = Res;
							if((USART_RX_STA&0x00FF) == 0) USART_RX_STA|= 0x1000;
						}
					}
				}
				else
				{
					if(Res == 0x01) 
					{
						USART_RX_BUF[1] = Res;
						USART_RX_STA |= 0x2000;
						USART_RX_STA += 4;		//оƬ��ַ����4
					}
					else USART_RX_STA = 0;
				}
			}
			else
			{
				if(Res == 0xEF) 
				{
					USART_RX_BUF[0] = Res;
					USART_RX_STA |= 0x4000;
				}
			}
		}
	}
	
	
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
	HAL_UART_Receive_IT(&huart2, (uint8_t *)USART_RX_BUF, 1);
  /* USER CODE END USART2_IRQn 1 */
}




// ָ��ʶ���ж�
unsigned char press_FR(void)
{
	extern SysPara AS608Para;
	extern uint16_t ValidN;
	SearchResult seach;
	uint8_t ensure;
	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
			ensure=PS_Search(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//�����ɹ�
			{		
				OLED_CLS();
				OLED_ShowStr(20,4,"Welcome!",1);
				if(seach.pageID>10)
					{return 2;}
				else
					{return 1;}
			}
			else
			{
				OLED_CLS();
				OLED_ShowStr(43,4,"Error!",1);
				HAL_Delay(1000);
				OLED_CLS();
			}				
	  }
		else ShowErrMessage(ensure);
	 HAL_Delay(600);
	}
	return 0;
		
}


//���ָ��
void Add_FR(void)
{
	uint8_t i,ensure ,processnum=0;
	uint8_t store_flag = 0;
	static uint8_t Pre_processnum = 99;
	uint16_t ID;
	extern SysPara AS608Para;
	extern uint16_t ValidN;
	extern unsigned char KEY;
	while(1)
	{
		switch (processnum)
		{
			case 0:
				i++;
				if(Pre_processnum!=processnum)	OLED_CLS();
				Pre_processnum = processnum;
				OLED_ShowStr(10,4,"Press Fingerprint!",1);
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer1);//��������
					if(ensure==0x00)
					{
						OLED_CLS();
						OLED_ShowStr(20,4,"Fingerprint OK!",1);
						i=0;
						processnum=1;//�����ڶ���						
					}
				}	
				break;
			
			case 1:
				i++;
				if(Pre_processnum!=processnum)	OLED_CLS();
				Pre_processnum = processnum;
				OLED_ShowStr(30,4,"Press Again!",1);
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer2);//��������
					if(ensure==0x00)
					{
						OLED_CLS();
						OLED_ShowStr(20,4,"Fingerprint OK!",1);
						i=0;
						processnum=2;//����������
					}
				}
				break;

			case 2:
				if(Pre_processnum!=processnum)	OLED_CLS();
				Pre_processnum = processnum;
				OLED_ShowStr(10,4,"Compare Fingerprint",1);
				ensure=PS_Match();
				if(ensure==0x00) 
				{
				OLED_CLS();
				OLED_ShowStr(20,4,"Fingerprint OK!",1);
					processnum=3;//�������Ĳ�
				}
				else 
				{
					OLED_CLS();
					OLED_ShowStr(0,4,"Fingerprint Error!",1);
					i=0;
					processnum=0;//���ص�һ��		
				}
				HAL_Delay(1200);
				break;

			case 3:
				OLED_CLS();
				OLED_ShowStr(10,4,"Generate Template",1);
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
					OLED_CLS();
					OLED_ShowStr(0,4,"Fingerprint Temp OK!",1);
					processnum=4;//�������岽
				}else {processnum=0;}
				HAL_Delay(1200);
				break;
				
			case 4:	
//				LCD_Fill(0,100,lcddev.width,160,WHITE);
//				Show_Str_Mid(0,100,"�����봢��ID,��Enter����",16,240);
//				Show_Str_Mid(0,120,"0=< ID <=299",16,240);
				OLED_CLS();
				ID=0;
				do{
					OLED_ShowStr(0,4,"Fingerprint ID :",1);
					OLED_ShowInt(100,4,ID,1);
					switch(KEY)
					{
						case 0x01: 
							if(ID<AS608Para.PS_max) ID++; 
							KEY = 0;
							break;
						case 0x02:    
							if(ID>0) ID--;
							KEY = 0;
							break;
						case 0x03:                                           
							store_flag = 1;
							KEY = 0;
							break;
						case 0x04:                                           
							store_flag = 1;
							KEY = 0;
							break;
						default:break;
					}
				
				}
				while((ID<AS608Para.PS_max) && store_flag ==0);//����ID����С��ָ�����������ֵ
				ensure=PS_StoreChar(CharBuffer2,ID);//����ģ��
				
				
				if(ensure==0x00) 
				{			
					OLED_CLS();
					OLED_ShowStr(30,4,"Store OK!",1);
					PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
					
					HAL_Delay(2000);
					OLED_CLS();
					return ;
				}else {processnum=0;}					
				break;				
		}
		HAL_Delay(400);
		if(i==5)//����5��û�а���ָ���˳�
		{
			break;	
		}				
	}
}
//ɾ��ָ��
void Del_FR(void)
{
	uint8_t  ensure;
	uint16_t ID;
	uint8_t store_flag = 0;
	OLED_CLS();
	ID=0;
	do{
		OLED_ShowStr(15,2,"Del Fingerprint",1);
		OLED_ShowStr(0,4,"Fingerprint ID :",1);
		OLED_ShowInt(100,4,ID,1);
		switch(KEY)
		{
			case 0x01: 
				if(ID<AS608Para.PS_max) ID++; 
				KEY = 0;
				break;
			case 0x02:    
				if(ID>0) ID--;
				KEY = 0;
				break;
			case 0x03:                                           
				store_flag = 1;
				KEY = 0;
				break;
			case 0x04:                                           
				store_flag = 1;
				KEY = 0;
				break;
			default:break;
		}
	
	}
	while((ID<AS608Para.PS_max) && store_flag ==0);//����ID����С��ָ�����������ֵ
	ensure=PS_DeletChar(ID,1);//ɾ������ָ��
	if(ensure==0)
	{
		OLED_CLS();
		OLED_ShowStr(20,4,"Del Succesful",1);
	}
  else ShowErrMessage(ensure);	
	HAL_Delay(1200);
	OLED_CLS();
	PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
}

uint32_t AS608Addr = 0XFFFFFFFF; //Ĭ��


//���ڷ���һ���ֽ�
static void MYUSART_SendData(uint8_t data)
{
	while((USART2->SR&0X40)==0); 
	USART2->DR = data;
}
//���Ͱ�ͷ
static void SendHead(void)
{
	MYUSART_SendData(0xEF);
	MYUSART_SendData(0x01);
}
//���͵�ַ
static void SendAddr(void)
{
	MYUSART_SendData(AS608Addr>>24);
	MYUSART_SendData(AS608Addr>>16);
	MYUSART_SendData(AS608Addr>>8);
	MYUSART_SendData(AS608Addr);
}
//���Ͱ���ʶ,
static void SendFlag(uint8_t flag)
{
	MYUSART_SendData(flag);
}
//���Ͱ�����
static void SendLength(int length)
{
	MYUSART_SendData(length>>8);
	MYUSART_SendData(length);
}
//����ָ����
static void Sendcmd(uint8_t cmd)
{
	MYUSART_SendData(cmd);
}
//����У���
static void SendCheck(uint16_t check)
{
	MYUSART_SendData(check>>8);
	MYUSART_SendData(check);
}

uint8_t Answer_Check(uint16_t waittime)
{
	extern uint8_t USART_RX_BUF[200];
	extern uint16_t USART_RX_STA;
	while(--waittime)
	{
		HAL_Delay(1);
		if(USART_RX_STA&0X8000)//���յ�һ������
		{
			USART_RX_STA=0;
			if(USART_RX_BUF[6] == 0x07) return USART_RX_BUF[9];
		}
	}
	return 0xFF;
}

//¼��ͼ�� PS_GetImage
//����:̽����ָ��̽�⵽��¼��ָ��ͼ�����ImageBuffer�� 
//ģ�鷵��ȷ����
uint8_t PS_GetImage(void)
{
	extern uint8_t USART_RX_BUF[200];
	extern uint16_t USART_RX_STA;
  uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x01);
  temp =  0x01+0x03+0x01;
	SendCheck(temp);
	ensure=Answer_Check(500);
	if(ensure!=0xff)
		ensure=USART_RX_BUF[9];
	else
		ensure=0xff;
	return ensure;
}
//�������� PS_GenChar
//����:��ImageBuffer�е�ԭʼͼ������ָ�������ļ�����CharBuffer1��CharBuffer2			 
//����:BufferID --> charBuffer1:0x01	charBuffer1:0x02												
//ģ�鷵��ȷ����
uint8_t PS_GenChar(uint8_t BufferID)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x04);
	Sendcmd(0x02);
	MYUSART_SendData(BufferID);
	temp = 0x01+0x04+0x02+BufferID;
	SendCheck(temp);
	ensure=Answer_Check(500);
	if(ensure!=0xff)
		ensure=USART_RX_BUF[9];
	else
		ensure=0xff;
	return ensure;
}
//��ȷ�ȶ���öָ������ PS_Match
//����:��ȷ�ȶ�CharBuffer1 ��CharBuffer2 �е������ļ� 
//ģ�鷵��ȷ����
uint8_t PS_Match(void)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x03);
	temp = 0x01+0x03+0x03;
	SendCheck(temp);
	ensure=Answer_Check(500);
	if(ensure!=0xff)
		ensure=USART_RX_BUF[9];
	else
		ensure=0xff;
	return ensure;
}
//����ָ�� PS_Search
//����:��CharBuffer1��CharBuffer2�е������ļ����������򲿷�ָ�ƿ�.�����������򷵻�ҳ�롣			
//����:  BufferID @ref CharBuffer1	CharBuffer2
//˵��:  ģ�鷵��ȷ���֣�ҳ�루����ָ��ģ�壩
uint8_t PS_Search(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x08);
	Sendcmd(0x04);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(StartPage>>8);
	MYUSART_SendData(StartPage);
	MYUSART_SendData(PageNum>>8);
	MYUSART_SendData(PageNum);
	temp = 0x01+0x08+0x04+BufferID
	+(StartPage>>8)+(uint8_t)StartPage
	+(PageNum>>8)+(uint8_t)PageNum;
	SendCheck(temp);
	
	ensure=Answer_Check(500);
	if(ensure!=0xff)
	{
		ensure=USART_RX_BUF[9];
		p->pageID   =(USART_RX_BUF[10]<<8)+USART_RX_BUF[11];
		p->mathscore=(USART_RX_BUF[12]<<8)+USART_RX_BUF[13];	
	}
	else
		ensure=0xff;
	return ensure;	
}
//�ϲ�����������ģ�壩PS_RegModel
//����:��CharBuffer1��CharBuffer2�е������ļ��ϲ����� ģ��,�������CharBuffer1��CharBuffer2	
//˵��:  ģ�鷵��ȷ����
uint8_t PS_RegModel(void)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x05);
	temp = 0x01+0x03+0x05;
	SendCheck(temp);
	ensure=Answer_Check(500);
	if(ensure!=0xff)
		ensure=USART_RX_BUF[9];
	else
		ensure=0xff;
	return ensure;		
}
//����ģ�� PS_StoreChar
//����:�� CharBuffer1 �� CharBuffer2 �е�ģ���ļ��浽 PageID ��flash���ݿ�λ�á�			
//����:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID��ָ�ƿ�λ�úţ�
//˵��:  ģ�鷵��ȷ����
uint8_t PS_StoreChar(uint8_t BufferID,uint16_t PageID)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x06);
	Sendcmd(0x06);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(PageID>>8);
	MYUSART_SendData(PageID);
	temp = 0x01+0x06+0x06+BufferID
	+(PageID>>8)+(uint8_t)PageID;
	SendCheck(temp);
	ensure=Answer_Check(500);
	if(ensure!=0xff)
		ensure=USART_RX_BUF[9];
	else
		ensure=0xff;
	return ensure;	
}
//ɾ��ģ�� PS_DeletChar
//����:  ɾ��flash���ݿ���ָ��ID�ſ�ʼ��N��ָ��ģ��
//����:  PageID(ָ�ƿ�ģ���)��Nɾ����ģ�������
//˵��:  ģ�鷵��ȷ����
uint8_t PS_DeletChar(uint16_t PageID,uint16_t N)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x07);
	Sendcmd(0x0C);
	MYUSART_SendData(PageID>>8);
	MYUSART_SendData(PageID);
	MYUSART_SendData(N>>8);
	MYUSART_SendData(N);
	temp = 0x01+0x07+0x0C
	+(PageID>>8)+(uint8_t)PageID
	+(N>>8)+(uint8_t)N;
	SendCheck(temp);
	ensure=Answer_Check(500);
	if(ensure!=0xff)
		ensure=USART_RX_BUF[9];
	else
		ensure=0xff;
	return ensure;
}
//���ָ�ƿ� PS_Empty
//����:  ɾ��flash���ݿ�������ָ��ģ��
//����:  ��
//˵��:  ģ�鷵��ȷ����
uint8_t PS_Empty(void)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x0D);
	temp = 0x01+0x03+0x0D;
	SendCheck(temp);
	ensure=Answer_Check(500);
	if(ensure!=0xff)
		ensure=USART_RX_BUF[9];
	else
		ensure=0xff;
	return ensure;
}
//дϵͳ�Ĵ��� PS_WriteReg
//����:  дģ��Ĵ���
//����:  �Ĵ������RegNum:4\5\6
//˵��:  ģ�鷵��ȷ����
uint8_t PS_WriteReg(uint8_t RegNum,uint8_t DATA)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x05);
	Sendcmd(0x0E);
	MYUSART_SendData(RegNum);
	MYUSART_SendData(DATA);
	temp = RegNum+DATA+0x01+0x05+0x0E;
	SendCheck(temp);
	ensure=Answer_Check(500);
	if(ensure!=0xff)
		ensure=USART_RX_BUF[9];
	else
		ensure=0xff;
//	if(ensure==0)
//		printf("\r\n���ò����ɹ���");
//	else
//		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//��ϵͳ�������� PS_ReadSysPara
//����:  ��ȡģ��Ļ��������������ʣ�����С��)
//����:  ��
//˵��:  ģ�鷵��ȷ���� + ����������16bytes��
uint8_t PS_ReadSysPara(SysPara *p)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x0F);
	temp = 0x01+0x03+0x0F;
	SendCheck(temp);
	ensure = Answer_Check(500);
	if(ensure != 0xFF)
	{
		ensure = USART_RX_BUF[9];
		p->PS_max = (USART_RX_BUF[14]<<8)+USART_RX_BUF[15];
		p->PS_level = USART_RX_BUF[17];
		p->PS_addr=(USART_RX_BUF[18]<<24)+(USART_RX_BUF[19]<<16)+(USART_RX_BUF[20]<<8)+USART_RX_BUF[21];
		p->PS_size = USART_RX_BUF[23];
		p->PS_N = USART_RX_BUF[25];
	}		
	else ensure=0xff;
//	if(ensure==0x00)
//	{
//		printf("\r\nģ�����ָ������=%d",p->PS_max);
//		printf("\r\n�Աȵȼ�=%d",p->PS_level);
//		printf("\r\n��ַ=%x",p->PS_addr);
//		printf("\r\n������=%d",p->PS_N*9600);
//	}
//	else 
//			printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//����ģ���ַ PS_SetAddr
//����:  ����ģ���ַ
//����:  PS_addr
//˵��:  ģ�鷵��ȷ����
uint8_t PS_SetAddr(uint32_t PS_addr)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x07);
	Sendcmd(0x15);
	MYUSART_SendData(PS_addr>>24);
	MYUSART_SendData(PS_addr>>16);
	MYUSART_SendData(PS_addr>>8);
	MYUSART_SendData(PS_addr);
	temp = 0x01+0x07+0x15
	+(uint8_t)(PS_addr>>24)+(uint8_t)(PS_addr>>16)
	+(uint8_t)(PS_addr>>8) +(uint8_t)PS_addr;				
	SendCheck(temp);
	AS608Addr=PS_addr;//������ָ�������ַ
	ensure=Answer_Check(500);
	if(ensure!=0xff)
		ensure=USART_RX_BUF[9];
	else
		ensure=0xff;
		AS608Addr = PS_addr;
//	if(ensure==0x00)
//		printf("\r\n���õ�ַ�ɹ���");
//	else
//		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//���ܣ� ģ���ڲ�Ϊ�û�������256bytes��FLASH�ռ����ڴ��û����±�,
//	�ü��±��߼��ϱ��ֳ� 16 ��ҳ��
//����:  NotePageNum(0~15),Byte32(Ҫд�����ݣ�32���ֽ�)
//˵��:  ģ�鷵��ȷ����
uint8_t PS_WriteNotepad(uint8_t NotePageNum,uint8_t *Byte32)
{
	uint16_t temp;
  uint8_t  ensure,i;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(36);
	Sendcmd(0x18);
	MYUSART_SendData(NotePageNum);
	for(i=0;i<32;i++)
	 {
		 MYUSART_SendData(Byte32[i]);
		 temp += Byte32[i];
	 }
  temp =0x01+36+0x18+NotePageNum+temp;
	SendCheck(temp);
	ensure=Answer_Check(500);
	if(ensure!=0xff)
		ensure=USART_RX_BUF[9];
	else
		ensure=0xff;
	return ensure;
}
//������PS_ReadNotepad
//���ܣ�  ��ȡFLASH�û�����128bytes����
//����:  NotePageNum(0~15)
//˵��:  ģ�鷵��ȷ����+�û���Ϣ
uint8_t PS_ReadNotepad(uint8_t NotePageNum,uint8_t *Byte32)
{
	uint16_t temp;
  uint8_t  ensure,i;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x04);
	Sendcmd(0x19);
	MYUSART_SendData(NotePageNum);
	temp = 0x01+0x04+0x19+NotePageNum;
	SendCheck(temp);
  ensure=Answer_Check(500);
	if(ensure!=0xff)
	{
		for(i=0;i<32;i++)
		{
			Byte32[i]=USART_RX_BUF[10+i];
		}
	}
	else
		ensure=0xff;
	return ensure;
}
//��������PS_HighSpeedSearch
//���ܣ��� CharBuffer1��CharBuffer2�е������ļ��������������򲿷�ָ�ƿ⡣
//		  �����������򷵻�ҳ��,��ָ����ڵ�ȷ������ָ�ƿ��� ���ҵ�¼ʱ����
//		  �ܺõ�ָ�ƣ���ܿ�������������
//����:  BufferID�� StartPage(��ʼҳ)��PageNum��ҳ����
//˵��:  ģ�鷵��ȷ����+ҳ�루����ָ��ģ�壩
uint8_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x08);
	Sendcmd(0x1b);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(StartPage>>8);
	MYUSART_SendData(StartPage);
	MYUSART_SendData(PageNum>>8);
	MYUSART_SendData(PageNum);
	temp = 0x01+0x08+0x1b+BufferID
	+(StartPage>>8)+(uint8_t)StartPage
	+(PageNum>>8)+(uint8_t)PageNum;
	SendCheck(temp);
	ensure=Answer_Check(500);
 	if(ensure!=0xff)
	{
		p->pageID 	=(USART_RX_BUF[10]<<8) +USART_RX_BUF[11];
		p->mathscore=(USART_RX_BUF[12]<<8) +USART_RX_BUF[13];
	}
	else
		ensure=0xff;
	return ensure;
}
//����Чģ����� PS_ValidTempleteNum
//���ܣ�����Чģ�����
//����: ��
//˵��: ģ�鷵��ȷ����+��Чģ�����ValidN
uint8_t PS_ValidTempleteNum(uint16_t *ValidN)
{	
	extern uint8_t USART_RX_BUF[200];
	extern uint16_t USART_RX_STA;
	
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x1d);
	temp = 0x01+0x03+0x1d;
	SendCheck(temp);
	ensure = Answer_Check(500);
	if(ensure != 0xFF)
	{
		*ValidN = (USART_RX_BUF[10]<<8) +USART_RX_BUF[11];
	}		
	else ensure=0xff;
	
	
//	if(ensure==0x00)
//	{
//		printf("\r\n��Чָ�Ƹ���=%d",(data[10]<<8)+data[11]);
//	}
//	else
//		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//��AS608���� PS_HandShake
//����: PS_Addr��ַָ��
//˵��: ģ�鷵�µ�ַ����ȷ��ַ��	
uint8_t PS_HandShake(uint32_t *PS_Addr)
{
	extern uint8_t USART_RX_BUF[200];
	extern uint16_t USART_RX_STA;
	SendHead();
	SendAddr();
	MYUSART_SendData(0X01);
	MYUSART_SendData(0X00);
	MYUSART_SendData(0X00);	
	HAL_Delay(200);
	
	if(USART_RX_STA&0X8000)//���յ�����
	{		
		if(//�ж��ǲ���ģ�鷵�ص�Ӧ���				
					USART_RX_BUF[0]==0XEF
				&&USART_RX_BUF[1]==0X01
				&&USART_RX_BUF[6]==0X07
			)
			{
				*PS_Addr=(USART_RX_BUF[2]<<24) + (USART_RX_BUF[3]<<16)
								+(USART_RX_BUF[4]<<8) + (USART_RX_BUF[5]);
				USART_RX_STA=0;
				return 0;
			}
		USART_RX_STA=0;					
	}
	return 1;		
}
//ģ��Ӧ���ȷ������Ϣ����
//���ܣ�����ȷ���������Ϣ������Ϣ
//����: ensure
const char *EnsureMessage(uint8_t ensure) 
{
	const char *p;
	switch(ensure)
	{
		case  0x00:
			p="OK";break;		
		case  0x01:
			p="���ݰ����մ���";break;
		case  0x02:
			p="��������û����ָ";break;
		case  0x03:
			p="¼��ָ��ͼ��ʧ��";break;
		case  0x04:
			p="ָ��ͼ��̫�ɡ�̫��������������";break;
		case  0x05:
			p="ָ��ͼ��̫ʪ��̫��������������";break;
		case  0x06:
			p="ָ��ͼ��̫�Ҷ�����������";break;
		case  0x07:
			p="ָ��ͼ����������������̫�٣������̫С��������������";break;
		case  0x08:
			p="ָ�Ʋ�ƥ��";break;
		case  0x09:
			p="û������ָ��";break;
		case  0x0a:
			p="�����ϲ�ʧ��";break;
		case  0x0b:
			p="����ָ�ƿ�ʱ��ַ��ų���ָ�ƿⷶΧ";
		case  0x10:
			p="ɾ��ģ��ʧ��";break;
		case  0x11:
			p="���ָ�ƿ�ʧ��";break;	
		case  0x15:
			p="��������û����Чԭʼͼ��������ͼ��";break;
		case  0x18:
			p="��д FLASH ����";break;
		case  0x19:
			p="δ�������";break;
		case  0x1a:
			p="��Ч�Ĵ�����";break;
		case  0x1b:
			p="�Ĵ����趨���ݴ���";break;
		case  0x1c:
			p="���±�ҳ��ָ������";break;
		case  0x1f:
			p="ָ�ƿ���";break;
		case  0x20:
			p="��ַ����";break;
		default :
			p="ģ�鷵��ȷ��������";break;
	}
 return p;	
}


