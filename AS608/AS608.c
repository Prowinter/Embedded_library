#include "AS608.h"
extern uint8_t USART_RX_BUF[200];
extern uint16_t USART_RX_STA;

//AS608 协议解析
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	
	/// 指纹模块传输协议解析
	
	extern uint8_t USART_RX_BUF[200];
	extern uint16_t USART_RX_STA;
	static uint8_t tem=0;
	uint8_t Res;	    
	if((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART2->DR;//(USART1->DR);	//读取接收到的数据
		if((USART_RX_STA&0x8000)==0) //接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0xEF
			{
				if(USART_RX_STA&0x2000)//接收到了0x01
				{
					if(USART_RX_STA&0x1000)//接收芯片地址
					{
						if(USART_RX_STA&0x0800)//接收包标识
						{
							if(USART_RX_STA&0x0400)//接收第一位包长度
							{
								if(USART_RX_STA&0x0200)//接收第二位包长度
								{
									if(USART_RX_STA&0x0100)//接收到最后一位数据
									{
										USART_RX_BUF[8+USART_RX_BUF[7]+USART_RX_BUF[8]] = Res;
										tem++;
										//和检验
										uint16_t sum = 0;
										for(int i=0;i<(1+tem);i++)
										{
											sum += USART_RX_BUF[6+i];
										}
										if(USART_RX_BUF[8]==0x19 && USART_RX_BUF[6]==0x07) sum-=6;
										//通过
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
									USART_RX_STA += Res; // 包长度
									tem = 0;
								}
							}
							else
							{
								USART_RX_BUF[7] = Res;
								USART_RX_STA |= 0x0400;
								USART_RX_STA += Res; // 包长度
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
						USART_RX_STA += 4;		//芯片地址长度4
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




// 指纹识别判断
unsigned char press_FR(void)
{
	extern SysPara AS608Para;
	extern uint16_t ValidN;
	SearchResult seach;
	uint8_t ensure;
	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//获取图像成功 
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{		
			ensure=PS_Search(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//搜索成功
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


//添加指纹
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
					ensure=PS_GenChar(CharBuffer1);//生成特征
					if(ensure==0x00)
					{
						OLED_CLS();
						OLED_ShowStr(20,4,"Fingerprint OK!",1);
						i=0;
						processnum=1;//跳到第二步						
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
					ensure=PS_GenChar(CharBuffer2);//生成特征
					if(ensure==0x00)
					{
						OLED_CLS();
						OLED_ShowStr(20,4,"Fingerprint OK!",1);
						i=0;
						processnum=2;//跳到第三步
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
					processnum=3;//跳到第四步
				}
				else 
				{
					OLED_CLS();
					OLED_ShowStr(0,4,"Fingerprint Error!",1);
					i=0;
					processnum=0;//跳回第一步		
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
					processnum=4;//跳到第五步
				}else {processnum=0;}
				HAL_Delay(1200);
				break;
				
			case 4:	
//				LCD_Fill(0,100,lcddev.width,160,WHITE);
//				Show_Str_Mid(0,100,"请输入储存ID,按Enter保存",16,240);
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
				while((ID<AS608Para.PS_max) && store_flag ==0);//输入ID必须小于指纹容量的最大值
				ensure=PS_StoreChar(CharBuffer2,ID);//储存模板
				
				
				if(ensure==0x00) 
				{			
					OLED_CLS();
					OLED_ShowStr(30,4,"Store OK!",1);
					PS_ValidTempleteNum(&ValidN);//读库指纹个数
					
					HAL_Delay(2000);
					OLED_CLS();
					return ;
				}else {processnum=0;}					
				break;				
		}
		HAL_Delay(400);
		if(i==5)//超过5次没有按手指则退出
		{
			break;	
		}				
	}
}
//删除指纹
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
	while((ID<AS608Para.PS_max) && store_flag ==0);//输入ID必须小于指纹容量的最大值
	ensure=PS_DeletChar(ID,1);//删除单个指纹
	if(ensure==0)
	{
		OLED_CLS();
		OLED_ShowStr(20,4,"Del Succesful",1);
	}
  else ShowErrMessage(ensure);	
	HAL_Delay(1200);
	OLED_CLS();
	PS_ValidTempleteNum(&ValidN);//读库指纹个数
}

uint32_t AS608Addr = 0XFFFFFFFF; //默认


//串口发送一个字节
static void MYUSART_SendData(uint8_t data)
{
	while((USART2->SR&0X40)==0); 
	USART2->DR = data;
}
//发送包头
static void SendHead(void)
{
	MYUSART_SendData(0xEF);
	MYUSART_SendData(0x01);
}
//发送地址
static void SendAddr(void)
{
	MYUSART_SendData(AS608Addr>>24);
	MYUSART_SendData(AS608Addr>>16);
	MYUSART_SendData(AS608Addr>>8);
	MYUSART_SendData(AS608Addr);
}
//发送包标识,
static void SendFlag(uint8_t flag)
{
	MYUSART_SendData(flag);
}
//发送包长度
static void SendLength(int length)
{
	MYUSART_SendData(length>>8);
	MYUSART_SendData(length);
}
//发送指令码
static void Sendcmd(uint8_t cmd)
{
	MYUSART_SendData(cmd);
}
//发送校验和
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
		if(USART_RX_STA&0X8000)//接收到一次数据
		{
			USART_RX_STA=0;
			if(USART_RX_BUF[6] == 0x07) return USART_RX_BUF[9];
		}
	}
	return 0xFF;
}

//录入图像 PS_GetImage
//功能:探测手指，探测到后录入指纹图像存于ImageBuffer。 
//模块返回确认字
uint8_t PS_GetImage(void)
{
	extern uint8_t USART_RX_BUF[200];
	extern uint16_t USART_RX_STA;
  uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//生成特征 PS_GenChar
//功能:将ImageBuffer中的原始图像生成指纹特征文件存于CharBuffer1或CharBuffer2			 
//参数:BufferID --> charBuffer1:0x01	charBuffer1:0x02												
//模块返回确认字
uint8_t PS_GenChar(uint8_t BufferID)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//精确比对两枚指纹特征 PS_Match
//功能:精确比对CharBuffer1 与CharBuffer2 中的特征文件 
//模块返回确认字
uint8_t PS_Match(void)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//搜索指纹 PS_Search
//功能:以CharBuffer1或CharBuffer2中的特征文件搜索整个或部分指纹库.若搜索到，则返回页码。			
//参数:  BufferID @ref CharBuffer1	CharBuffer2
//说明:  模块返回确认字，页码（相配指纹模板）
uint8_t PS_Search(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//合并特征（生成模板）PS_RegModel
//功能:将CharBuffer1与CharBuffer2中的特征文件合并生成 模板,结果存于CharBuffer1与CharBuffer2	
//说明:  模块返回确认字
uint8_t PS_RegModel(void)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//储存模板 PS_StoreChar
//功能:将 CharBuffer1 或 CharBuffer2 中的模板文件存到 PageID 号flash数据库位置。			
//参数:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID（指纹库位置号）
//说明:  模块返回确认字
uint8_t PS_StoreChar(uint8_t BufferID,uint16_t PageID)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//删除模板 PS_DeletChar
//功能:  删除flash数据库中指定ID号开始的N个指纹模板
//参数:  PageID(指纹库模板号)，N删除的模板个数。
//说明:  模块返回确认字
uint8_t PS_DeletChar(uint16_t PageID,uint16_t N)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//清空指纹库 PS_Empty
//功能:  删除flash数据库中所有指纹模板
//参数:  无
//说明:  模块返回确认字
uint8_t PS_Empty(void)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//写系统寄存器 PS_WriteReg
//功能:  写模块寄存器
//参数:  寄存器序号RegNum:4\5\6
//说明:  模块返回确认字
uint8_t PS_WriteReg(uint8_t RegNum,uint8_t DATA)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//		printf("\r\n设置参数成功！");
//	else
//		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//读系统基本参数 PS_ReadSysPara
//功能:  读取模块的基本参数（波特率，包大小等)
//参数:  无
//说明:  模块返回确认字 + 基本参数（16bytes）
uint8_t PS_ReadSysPara(SysPara *p)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//		printf("\r\n模块最大指纹容量=%d",p->PS_max);
//		printf("\r\n对比等级=%d",p->PS_level);
//		printf("\r\n地址=%x",p->PS_addr);
//		printf("\r\n波特率=%d",p->PS_N*9600);
//	}
//	else 
//			printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//设置模块地址 PS_SetAddr
//功能:  设置模块地址
//参数:  PS_addr
//说明:  模块返回确认字
uint8_t PS_SetAddr(uint32_t PS_addr)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
	AS608Addr=PS_addr;//发送完指令，更换地址
	ensure=Answer_Check(500);
	if(ensure!=0xff)
		ensure=USART_RX_BUF[9];
	else
		ensure=0xff;
		AS608Addr = PS_addr;
//	if(ensure==0x00)
//		printf("\r\n设置地址成功！");
//	else
//		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//功能： 模块内部为用户开辟了256bytes的FLASH空间用于存用户记事本,
//	该记事本逻辑上被分成 16 个页。
//参数:  NotePageNum(0~15),Byte32(要写入内容，32个字节)
//说明:  模块返回确认字
uint8_t PS_WriteNotepad(uint8_t NotePageNum,uint8_t *Byte32)
{
	uint16_t temp;
  uint8_t  ensure,i;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//读记事PS_ReadNotepad
//功能：  读取FLASH用户区的128bytes数据
//参数:  NotePageNum(0~15)
//说明:  模块返回确认字+用户信息
uint8_t PS_ReadNotepad(uint8_t NotePageNum,uint8_t *Byte32)
{
	uint16_t temp;
  uint8_t  ensure,i;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//高速搜索PS_HighSpeedSearch
//功能：以 CharBuffer1或CharBuffer2中的特征文件高速搜索整个或部分指纹库。
//		  若搜索到，则返回页码,该指令对于的确存在于指纹库中 ，且登录时质量
//		  很好的指纹，会很快给出搜索结果。
//参数:  BufferID， StartPage(起始页)，PageNum（页数）
//说明:  模块返回确认字+页码（相配指纹模板）
uint8_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p)
{
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//读有效模板个数 PS_ValidTempleteNum
//功能：读有效模板个数
//参数: 无
//说明: 模块返回确认字+有效模板个数ValidN
uint8_t PS_ValidTempleteNum(uint16_t *ValidN)
{	
	extern uint8_t USART_RX_BUF[200];
	extern uint16_t USART_RX_STA;
	
	uint16_t temp;
  uint8_t  ensure;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//		printf("\r\n有效指纹个数=%d",(data[10]<<8)+data[11]);
//	}
//	else
//		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//与AS608握手 PS_HandShake
//参数: PS_Addr地址指针
//说明: 模块返新地址（正确地址）	
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
	
	if(USART_RX_STA&0X8000)//接收到数据
	{		
		if(//判断是不是模块返回的应答包				
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
//模块应答包确认码信息解析
//功能：解析确认码错误信息返回信息
//参数: ensure
const char *EnsureMessage(uint8_t ensure) 
{
	const char *p;
	switch(ensure)
	{
		case  0x00:
			p="OK";break;		
		case  0x01:
			p="数据包接收错误";break;
		case  0x02:
			p="传感器上没有手指";break;
		case  0x03:
			p="录入指纹图像失败";break;
		case  0x04:
			p="指纹图像太干、太淡而生不成特征";break;
		case  0x05:
			p="指纹图像太湿、太糊而生不成特征";break;
		case  0x06:
			p="指纹图像太乱而生不成特征";break;
		case  0x07:
			p="指纹图像正常，但特征点太少（或面积太小）而生不成特征";break;
		case  0x08:
			p="指纹不匹配";break;
		case  0x09:
			p="没搜索到指纹";break;
		case  0x0a:
			p="特征合并失败";break;
		case  0x0b:
			p="访问指纹库时地址序号超出指纹库范围";
		case  0x10:
			p="删除模板失败";break;
		case  0x11:
			p="清空指纹库失败";break;	
		case  0x15:
			p="缓冲区内没有有效原始图而生不成图像";break;
		case  0x18:
			p="读写 FLASH 出错";break;
		case  0x19:
			p="未定义错误";break;
		case  0x1a:
			p="无效寄存器号";break;
		case  0x1b:
			p="寄存器设定内容错误";break;
		case  0x1c:
			p="记事本页码指定错误";break;
		case  0x1f:
			p="指纹库满";break;
		case  0x20:
			p="地址错误";break;
		default :
			p="模块返回确认码有误";break;
	}
 return p;	
}


