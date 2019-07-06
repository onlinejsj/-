#include "ANjian.h"
#include "mylcd.h"

FLASH_DATA_INIT lastTimeData;//上次输入数据记录读取


uint8_t GaihaoArr[25] = {0xFE,0xFE,0xFE,0xFE,0x68,0x0E,0xF1,0x68,0xAA,0xAA,0xAA,0xAA,0x72,0x14,0x91,0x67,0x11,0x11,0x11,0x11,0x6A,0x16};//22 改表号命令
uint8_t QinglingArr[40] = {0xFE,0xFE,0xFE,0xFE,0x68,0x1D,0xE2,0x68,0xAA,0xAA,0xAA,0xAA,0x75,0x35,0x53,0x66,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0B,0x16};//37 清零
uint8_t GaiBtlArr[20] = {0xFF,0xFF,0x68,0x0C,0xF3,0x68,0xAA,0xAA,0xAA,0xAA,0x37,0x00,0x00,0x00,0x00,0x02,0xE6,0x16};//18
//uint8_t GaiXishuArr3[26] = {0xFE,0xFE,0xFE,0xFE,0x68,0x14,0xEB,0x68,0xAA,0xAA,0xAA,0xAA,0x36,0x98,0x52,0x66,0x00,0x05,0x00,0x07,0x00,0x09,0xD3,0x16}; //24	
uint8_t GaiXishuArr[30] ={0xFE,0xFE,0xFE,0xFE,0x68,0x14,0xEB,0x68,0xAA,0xAA,0xAA,0xAA,0x36,0x98,0x52,0x66,0x00,0x05,0x00,0x07,0x00,0x09,0x00,0x03,0x00,0x04,0xB6,0x16};//28 5段改系数
uint8_t Gaikoujing[20] = {0xFE,0xFE,0xFE,0xFE,0x68,0x0B,0xF4,0x68,0xAA,0xAA,0xAA,0xAA,0x35,0x39,0x46,0x93,0x0B,0x07,0x16};//19
	//uint8_t GaishijianArr_ModBus[] = {};
//uint8_t GaishijianArr_MBus[] = {};
uint8_t QiangZhiArr[22] = {0xFE,0xFE,0xFE,0xFE,0x68,0x41,0xAA,0xAA,0xAA,0xAA,0x04,0x07,0x17,0xA0,0x00,0x33,0x22,0x11,0x00,0x00,0x16};//21	
	



/*****************  功能按键处理函数 ******************/
void gongNengAnJianChuLi(void)
{
	static uint8_t gongPre=0;
	if(((Gongneng&0x80)==0x80))//发送按键
	{
		if(sendWay==YOUXIAN)
		{
			EN_LH(1);
		}
		LED_RCV(0);
		shengCheng_Send();
		Gongneng &=0x7F ; //功能键(发送)被响应			
		saveLastTimeData();			
		EN_LH(0);
		return;
	}
	else
	{			
		switch(GONGNENG_CHAXUN(Gongneng))
		{
			case BTLJYW:
				if(gongPre==BTLJYW)
				{
					if(lastTimeData.btlJyw==N9600)//修改波特率
					{
						lastTimeData.btlJyw = E2400;
					}
					else if(lastTimeData.btlJyw==E2400)
					{
						lastTimeData.btlJyw = N2400;
					}
					else if(lastTimeData.btlJyw==N2400)
					{
						lastTimeData.btlJyw = E9600;
					}
					else if(lastTimeData.btlJyw==E9600)
					{
						lastTimeData.btlJyw=N9600;
					}
				}
				break;
			case BIAOHAO:
				Jianzhihecheng = 0;
				break;
			//case SHIJIAN:
			case KOUJING:
				if(sendWay==HONGWAI)//此功能主要用于联创改表号使用
				{
					lastTimeData.ID--;
				}
				Jianzhihecheng = 0;
				break;
			case XISHU:
					if((Xishuhao==1)&&(Jianzhihecheng!=0))
					{
						lastTimeData.xs5 = Jianzhihecheng;
					}
					else if((Xishuhao==2)&&(Jianzhihecheng!=0))
					{
						lastTimeData.xs7 = Jianzhihecheng;
					}
					else if((Xishuhao==3)&&(Jianzhihecheng!=0))
					{
						lastTimeData.xs9 = Jianzhihecheng;
					}
					else if((Xishuhao==4)&&(Jianzhihecheng!=0))
					{
						lastTimeData.xs6 = Jianzhihecheng;
					}
					else if((Xishuhao==5)&&(Jianzhihecheng!=0))
					{
						lastTimeData.xs8 = Jianzhihecheng;
					}
					Xishuhao++;
					//if(Xishuhao>4)//循环一圈 5796
					if(Xishuhao>(lastTimeData.xsd))//循环一圈 57968
					{
						Xishuhao=1;
					}
					
					Jianzhihecheng=0;						
				break;
			case QINGLING:
				break;
			default:
				break;
		}
		GONGNENG_ZHIXING_XIANGYING(Gongneng); //功能键被响应
		if(gongPre!=GONGNENG_CHAXUN(Gongneng))
		{
			saveLastTimeData();
		}
		gongPre = GONGNENG_CHAXUN(Gongneng);//更新上一功能记录
	}
}



/*****************  生成发送指令函数 ******************/
void shengCheng_Send(void)	
{
	uint8_t j=0;
	uint32_t temp=0;
	startFlg = 0;
	memset(rcvArr,0,36);//清空接收数组	
	EnableNvic(PORTB_IRQn,IrqLevel3 ,FALSE); //响应发送按键后关闭按键扫描
	switch(GONGNENG_CHAXUN(Gongneng))
	{
		case BTLJYW:
			switch(lastTimeData.btlJyw)
			{
				case E2400:
					GaiBtlArr[14] = 0x00;
					GaiBtlArr[15] = 0x02;
					break;
				case N2400:
					GaiBtlArr[14] = 0x00;
					GaiBtlArr[15] = 0x00;
					break;
				case E9600:
					GaiBtlArr[14] = 0x01;
					GaiBtlArr[15] = 0x02;
					break;
				case N9600:
					GaiBtlArr[14] = 0x01;
					GaiBtlArr[15] = 0x00;
					break;
			}
			GaiBtlArr[16] = jiaoYanHe((GaiBtlArr+6),10);//此case以上是改波特率指令生成		
			if(sendWay==YOUXIAN)
			{
				uartSend_user_lp(GaiBtlArr,18);//改波特率发送
			}
			else
			{
				uartSend_user(GaiBtlArr,18);
			}
			break;
			
		case BIAOHAO:
			if(Jianzhihecheng!=0)
			{
				lastTimeData.ID = Jianzhihecheng;
				temp = Jianzhihecheng;	
			}
			else
			{
				temp = lastTimeData.ID;
			}
			for(j=0;j<4;j++)
			{
				GaihaoArr[(19-j)] = (temp%100)/10*6+(temp%100);
				temp /= 100;
			}
			GaihaoArr[20] = jiaoYanHe((GaihaoArr+8),12);
			if(sendWay==YOUXIAN)
			{
				uartSend_user_lp(GaihaoArr,22);//改表号发送				
			}
			else
			{
				uartSend_user(GaihaoArr,22);
			}
//			if(sendWay==HONGWAI)//红外功能主要用于联创改表号 自动＋1
//			{
				lastTimeData.ID++;
				valueDisplay(lastTimeData.ID,8,0);
				//
//			}
			break;
			
		//case SHIJIAN:
			case KOUJING:
				//disPlay_Clear(5,4);
				if(Jianzhihecheng!=0)
				{
					lastTimeData.kj=Jianzhihecheng;
				}
				switch(lastTimeData.kj)
				{
					case 15:
						Gaikoujing[16] = DN15;break;
					case 20:
						Gaikoujing[16] = DN20;break;
					case 25:
						Gaikoujing[16] = DN25;break;
					case 32:
						Gaikoujing[16] = DN32;break;
					case 40:
						Gaikoujing[16] = DN40;break;
					case 50:
						Gaikoujing[16] = DN50;break;
					case 65:
						Gaikoujing[16] = DN65;break;
					case 80:
						Gaikoujing[16] = DN80;break;
					case 100:
						Gaikoujing[16] = DN100;break;
					case 125:
						Gaikoujing[16] = DN125;break;
					case 150:
						Gaikoujing[16] = DN150;break;
					case 200:
						Gaikoujing[16] = DN200;break;
					case 250:
						Gaikoujing[16] = DN250;break;
					case 300:
						Gaikoujing[16] = DN300;break;
					case 350:
						Gaikoujing[16] = DN350;break;
					case 400:
						Gaikoujing[16] = DN400;break;
					case 450:
						Gaikoujing[16] = DN450;break;
					case 500:
						Gaikoujing[16] = DN500;break;
					case 550:
						Gaikoujing[16] = DN550;break;
					case 600:
						Gaikoujing[16] = DN600;break;
					case 700:
						Gaikoujing[16] = DN700;break;
					default:
						Jianzhihecheng=0;
//						koujinghao=1;
						EnableNvic(PORTB_IRQn,IrqLevel3 ,TRUE); //打开按键中断响应  
						return;
				}
				Gaikoujing[17] = jiaoYanHe((Gaikoujing+8),9);
				if(sendWay==YOUXIAN)
				{
					uartSend_user_lp(Gaikoujing,19);	
				}
				else
				{
					uartSend_user(Gaikoujing,19);
				}				
				break;
		
		case XISHU:
		  GaiXishuArr[16] = (lastTimeData.xs5>>8)&0xFF;
			GaiXishuArr[17] = lastTimeData.xs5&0xFF;
		  GaiXishuArr[18] = (lastTimeData.xs7>>8)&0xFF;
			GaiXishuArr[19] = lastTimeData.xs7&0xFF;
			GaiXishuArr[20] = (lastTimeData.xs9>>8)&0xFF;
			GaiXishuArr[21] = lastTimeData.xs9&0xFF;
			
		  GaiXishuArr[5]=0x14;//指令头包含长度变化
		  GaiXishuArr[6]=0xEB;	
		
			if(lastTimeData.xsd==XISHU_4D)
			{	
				GaiXishuArr[24] = (lastTimeData.xs6>>8)&0xFF;
				GaiXishuArr[25] = lastTimeData.xs6&0xFF;	
				GaiXishuArr[26] = jiaoYanHe((GaiXishuArr+8),18);		
				temp = 28;	
			}	
			else if(lastTimeData.xsd==XISHU_3D)
			{		
				GaiXishuArr[5]=0x10;
				GaiXishuArr[6]=0xEF;	
				GaiXishuArr[23]=0x16;	
				GaiXishuArr[22] = jiaoYanHe((GaiXishuArr+8),14);	
				temp = 24;	
			}
			else if(lastTimeData.xsd==XISHU_5D)
			{		
				GaiXishuArr[22] = (lastTimeData.xs6>>8)&0xFF;
				GaiXishuArr[23] = lastTimeData.xs6&0xFF;					
				GaiXishuArr[24] = (lastTimeData.xs8>>8)&0xFF;
				GaiXishuArr[25] = lastTimeData.xs8&0xFF;
				GaiXishuArr[26] = jiaoYanHe((GaiXishuArr+8),18);	
				temp = 28;	
			}	
			
			if(sendWay==YOUXIAN)
			{				
				uartSend_user_lp(GaiXishuArr,temp);	
			}
			else
			{
				uartSend_user(GaiXishuArr,temp);
			}
		  //Xishuhao=0;  //系数号重新置零
			break;
		
		case QINGLING:
			if(sendWay==YOUXIAN)
			{
				uartSend_user_lp(QinglingArr,37);//清零指令发送	
			}
			else
			{
				uartSend_user(QinglingArr,37);
			}				
			break;
		default:
			EnableNvic(PORTB_IRQn,IrqLevel3 ,TRUE); //打开按键中断响应  
			return;
	}
	Jianzhihecheng=0;		
}


/*************  lpuart发送函数  ******************/
void uartSend_user_lp(uint8_t *arr,uint8_t length)
{
		uint8_t i=0;	
		while(i<length)
		{
			
			LED_SEND(1);
			M0P_LPUART1->SBUF = arr[i];
			while(0 == M0P_LPUART1->ISR_f.TC); //发送未完成				
			M0P_LPUART1->ICR_f.TCCF = 0;
			i++;
			LED_SEND(0);
		}
		MAIN_T = OUT_TIME;
		MAIN_T_JISHI(MAIN_T);
		
			
	if((currentBtl&0xF0)==0x10)//9600时
	{
		lpowerFlg=0;//9600波特率时不立马进入低功耗	
	}
	EnableNvic(PORTB_IRQn,IrqLevel3 ,TRUE); //打开按键中断响应  
}

/*************  uart 红外发送函数  ******************/
void uartSend_user(uint8_t *arr,uint8_t length)
{
		uint8_t i=0;	
		while(i<length)
		{
			
			LED_SEND(1);
			M0P_UART1->SBUF = arr[i];
			while(0 == M0P_UART1->ISR_f.TC); //发送未完成				
			M0P_UART1->ICR_f.TCCF = 0;
			i++;
			LED_SEND(0);
		}
		MAIN_T = OUT_TIME;
		MAIN_T_JISHI(MAIN_T);
		
		lpowerFlg=0;//红外或者9600波特率时都不可进入低功耗
		EnableNvic(PORTB_IRQn,IrqLevel3 ,TRUE); //打开按键中断响应  
}
	

/************  从Flash读上次使用过的数值  ******************/
void readLastTimeData(void)
{
	lastTimeData.btlJyw = *((uint8_t *)(ADDR_LAST_BTLJYW));
	lastTimeData.xs5 = (*((uint16_t *)(ADDR_LAST_xs5)));
	lastTimeData.xs6 = (*((uint16_t *)(ADDR_LAST_xs7)));
	lastTimeData.xs7 = (*((uint16_t *)(ADDR_LAST_xs9)));
	lastTimeData.xs9 = (*((uint16_t *)(ADDR_LAST_xs6)));
	lastTimeData.ID = (*((uint32_t *)(ADDR_LAST_ID)));
	sendWay=lastTimeData.Ver = (*((uint8_t *)(ADDR_LAST_Ver)));
	lastTimeData.kj = (*((uint16_t *)(ADDR_LAST_KJ)));
	lastTimeData.xs8 = (*((uint16_t *)(ADDR_LAST_xs8)));
	xishu_duan = lastTimeData.xsd = (*((uint8_t *)(ADDR_LAST_XSD)));
}


/************  存Flash  上次使用过的数值  ******************/
void saveLastTimeData(void)
{
	Flash_SectorErase(0x8000);//先擦除
	Flash_WriteByte(ADDR_LAST_BTLJYW,lastTimeData.btlJyw);//存
	Flash_WriteWord(ADDR_LAST_ID,lastTimeData.ID);//存   半字写  地址已按字长对齐
	Flash_WriteByte(ADDR_LAST_Ver,lastTimeData.Ver);//存
	Flash_WriteHalfWord(ADDR_LAST_xs5,lastTimeData.xs5);//存
	Flash_WriteHalfWord(ADDR_LAST_xs7,lastTimeData.xs7);//存
	Flash_WriteHalfWord(ADDR_LAST_xs9,lastTimeData.xs9);//存
	Flash_WriteHalfWord(ADDR_LAST_xs6,lastTimeData.xs6);//存
	Flash_WriteHalfWord(ADDR_LAST_xs8,lastTimeData.xs8);//存
	Flash_WriteHalfWord(ADDR_LAST_KJ,lastTimeData.kj);//存
	Flash_WriteHalfWord(ADDR_LAST_XSD,lastTimeData.xsd);//存
}

/************  整理合成输入值    ******************/
void readAnJian_Hecheng(void)
{
		Jianzhihecheng = (Jianzhihecheng<<1)+(Jianzhihecheng<<3)+JIANZHI;
	
		switch(GONGNENG_CHAXUN(Gongneng))
		{
			case XISHU:
				if(Jianzhihecheng>0xFFFF)//如果系数输入数值大于65535  该项系数清零
				{
					Jianzhihecheng=JIANZHI;
					disPlay_Clear(2,10);
				}
				break;
			case BIAOHAO:
				if(Jianzhihecheng>99999999)//如果系数输入数值大于9999999  表号清零
				{
					Jianzhihecheng=JIANZHI;
					disPlay_Clear(1,11);
				}
				break;
			case KOUJING:
				if(Jianzhihecheng>700)
				{					
					Jianzhihecheng=JIANZHI;
					disPlay_Clear(5,4);
				}
				break;
			case QINGLING:
				Jianzhihecheng=0;
				break;			
		}		
}



/************  求校验和  ******************/
uint8_t jiaoYanHe(uint8_t *arr,uint8_t length)
{
	uint8_t sum_8t=0;
	uint8_t i;
	for(i=0;i<length;i++)
	{
		sum_8t += (*(arr+i));
	}	
	return sum_8t;
}



/************* 此设备 切换通讯参数 ****************/
void swpBtl(void)
{
	LPUart_DisableFunc(LPUART1,LPUartRx);
	currentBtl = lastTimeData.btlJyw;
	switch(lastTimeData.btlJyw)
	{
		case N9600:
			LPUart_SetMode(LPUART1,LPUartMode1);
		
			LPUart_SelSclk(LPUART1,LPUart_Pclk);//切换成高速时钟  main函数不能进入低功耗
			LPUart_SetBaud(LPUART1,104);
			//时钟为RCL时=LPUart_SetBaud(LPUART1,1);
			break;
		case E9600:
			LPUart_SetMode(LPUART1,LPUartMode3);
		
			LPUart_SelSclk(LPUART1,LPUart_Pclk);//切换成高速时钟  main函数不能进入低功耗
			LPUart_SetBaud(LPUART1,104);
			//LPUart_SetBaud(LPUART1,1);
			LPUart_SetMMDOrCk(LPUART1,LPUartEven);
			break;	
		case E2400:
			LPUart_SetMode(LPUART1,LPUartMode3);	
		
			LPUart_SelSclk(LPUART1,LPUart_Rcl);//切换成RCL  main函数可进入低功耗
		
			LPUart_SetBaud(LPUART1,4);
			LPUart_SetMMDOrCk(LPUART1,LPUartEven);
			break;	
		case N2400:
			LPUart_SetMode(LPUART1,LPUartMode1);
		
			LPUart_SelSclk(LPUART1,LPUart_Rcl);//切换成RCL  main函数可进入低功耗
		
			LPUart_SetBaud(LPUART1,4);
			break;			
	}
	saveLastTimeData();
	LPUart_EnableFunc(LPUART1,LPUartRx);
}


