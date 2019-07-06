
/******************************************************************************
 * Include files
 ******************************************************************************/
 
#include "init.h"
#include "irq.h"
#include "ANjian.h "
#include "mylcd.h"


//uint8_t GaihaoArr[25] = {0xFE,0xFE,0xFE,0xFE,0x68,0x0E,0xF1,0x68,0xAA,0xAA,0xAA,0xAA,0x72,0x14,0x91,0x67,0x11,0x11,0x11,0x11,0x6A,0x16};//22 改表号命令
////uint8_t GaixishuArr[45]	={0xFE,0xFE,0xFE,0xFE,0x68,0x22,0xDD,0x68,0x11,0x11,0x11,0x11,0x39,0x76,0x22,0x88,0x00,0x04,0x01,0x4F,0x56,0x01,0x00,0x00,0x00,0x65,0xA0,0x01,0x00,0x02,0x01,0x27,0x5E,0x01,0x00,0x02,0x00,0x3E,0x40,0x01,0x58,0x16};//42 改时间
//uint8_t QinglingArr[40] = {0xFE,0xFE,0xFE,0xFE,0x68,0x1D,0xE2,0x68,0xAA,0xAA,0xAA,0xAA,0x75,0x35,0x53,0x66,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0B,0x16};//37 清零
//uint8_t GaiBtlArr[20] = {0xFF,0xFF,0x68,0x0C,0xF3,0x68,0xAA,0xAA,0xAA,0xAA,0x37,0x00,0x00,0x00,0x00,0x02,0xE6,0x16};//18
//uint8_t GaiXishuArr[30] = {0xFE,0xFE,0xFE,0xFE,0x68,0x14,0xEB,0x68,0x12,0x34,0x56,0x78,0x36,0x98,0x52,0x66,0x00,0x05,0x00,0x07,0x00,0x09,0x0E,0x10,0x00,0x06,0xD3,0x16}; //28
////uint8_t GaishijianArr_ModBus[] = {};
////uint8_t GaishijianArr_MBus[] = {};
////uint8_t Gaishijian[] = {};
	

/******************************************************************************
 * main函数
 ******************************************************************************/
int32_t main(void)
{  	
	ClkInit();
	initIo();
	rtcInit();
	wdtInit();
	lpmInit();	
	LP_Uart_portInit();
	LP_Uart_configInit();
	Uart_portInit();
	Uart_configInit();
	Wdt_Start();//打开看门狗	
	
	readLastTimeData();//读FLASH完成后	
	swpBtl();
	
	Rtc_EnableFunc(RtcCount);//开启 RTC
  EnableNvic(PORTB_IRQn, IrqLevel3, TRUE);	///< 打开按键中断..
	lcdInit();
	
	LED_POWER(1);//电源灯亮  初始化完毕	
	
	if(lastTimeData.xsd==0xFF)
	{
		lastTimeData.xsd = XISHU_3D;
	}
	if(sendWay==0xFF)
	{
		sendWay = lastTimeData.Ver = YOUXIAN;
	}
	
	
	while(1)
	{		
		uint8_t successFlg=0;		
		
		//Wdt_Feed();
#if 1			
		if((MOSHICHAXUN(fuc_model_times))==XUANZHEMOSHI)
		{
			//若是选择模式，判断用户输入的功能编号
			if(GONGNENG_CHAXUN(Gongneng)!=XISHU)
			{
				disPlay_Clear(5,2);
			}
			switch((JIANZHI&0x0F))//判断用户选择的功能
			{
				case BTLJYW:	
					SEG[7]=0;
					Gongneng=((Gongneng&0xF0)|BTLJYW);
					valueDisplay(BTLJYW,8,0);
					break;
				
				case BIAOHAO:
					SEG[7]=0;
					Gongneng=((Gongneng&0xF0)|BIAOHAO);
					valueDisplay(BIAOHAO,8,0);
					break;
				case KOUJING:
					SEG[7]=0;
					Gongneng=((Gongneng&0xF0)|KOUJING);
					valueDisplay(KOUJING,8,0);
					break;
				case XISHU:
					SEG[7]=0;
					Gongneng=((Gongneng&0xF0)|XISHU);
					if(lastTimeData.xsd==XISHU_3D)
					{
						SEG[6]=G;SEG[5]=SHUZHI[3];
					}
					else if(lastTimeData.xsd==XISHU_4D)
					{
						SEG[6]=G;SEG[5]=SHUZHI[4];
					}
					else if(lastTimeData.xsd==XISHU_5D)
					{
						SEG[6]=G;SEG[5]=SHUZHI[5];
					}
					valueDisplay(XISHU,8,0);
					break;
				case QINGLING:
					SEG[7]=0;
					Gongneng=((Gongneng&0xF0)|QINGLING);
					SEG[8]=SHUZHI[0];
					xianShi();
					break;
				case YOUXIAN:
					sendWay=YOUXIAN;
					lastTimeData.Ver=YOUXIAN;
					valueDisplay(YOUXIAN,2,0);
					break;
				case HONGWAI:
					sendWay=HONGWAI;
					lastTimeData.Ver=HONGWAI;
					valueDisplay(HONGWAI,2,0);
					break;
				case XISHU_3D:
					Gongneng=((Gongneng&0xF0)|XISHU);
					lastTimeData.xsd=XISHU_3D;
					SEG[6]=G;SEG[5]=SHUZHI[3];
					valueDisplay(XISHU,8,0);
					break;
				case XISHU_4D:
					Gongneng=((Gongneng&0xF0)|XISHU);
					lastTimeData.xsd=XISHU_4D;
					SEG[6]=G;SEG[5]=SHUZHI[4];
					valueDisplay(XISHU,8,0);
					break;
				case XISHU_5D:
					Gongneng=((Gongneng&0xF0)|XISHU);
					lastTimeData.xsd=XISHU_5D;
					SEG[6]=G;SEG[5]=SHUZHI[5];
					valueDisplay(XISHU,8,0);
					break;
				default:
					break;
			}
		  //JIANZHI = 0xFF;
			if(((Gongneng&0x80)==0x80))//发送按键(此处应为确认按键)
			{
				Gongneng &=0x0F ; //确认功能	
				GONGNENG_QINGQIU_XIANGYING(Gongneng);//进入功能执行界面
				JIN_ZHANGCHANGMOSHI(fuc_model_times);
				lpowerFlg=1;				
			}
			saveLastTimeData();
		}
		else if((MOSHICHAXUN(fuc_model_times))==ZHENGCHANGMOSHI)
		{//}1
#endif
		
		
		
		
#if 1		
		//先判断用户选择什么功能,或者功能下的小选项
			if((GONGNENG_JIANCHA_XIANGYING(Gongneng)))//检测是否有待执行功能按键
			{
				gongNengAnJianChuLi();//发送按键也在功能中		
				//功能显示更新				
				userXianShi();
			}
			else if((JIANZHI<0x0A)&&(JIANZHI!=0xFF)&&(GONGNENG_CHAXUN(Gongneng))&&(GONGNENG_CHAXUN(Gongneng)!=BTLJYW))  //不是功能按键(数字按键) 切在某一功能设置里面
			{
				readAnJian_Hecheng();//合成数值				
				//数值显示更新
				if(GONGNENG_CHAXUN(Gongneng)==XISHU)//系数
				{
					disPlay_Clear(2,10);
				}	
				else if(GONGNENG_CHAXUN(Gongneng)==KOUJING)//口径
				{
					disPlay_Clear(5,4);
				}				
				else if(GONGNENG_CHAXUN(Gongneng)==QINGLING)//清零
				{
				}
				else
				{
					disPlay_Clear(1,11);
				}
				valueDisplay(Jianzhihecheng,8,1);
			}
			
		  JIANZHI = 0xFF;
#endif
		}//此括号是正常模式运行界限 }1
#if 1
			//处理接收数据并显示
		if(DEAL_QQ_CHAXUN(dataDealFlg))//若有-处理->判断处理结果->显示结果
		{	
			MAIN_T_CS_QING(MAIN_T);//计时停止并初始化
			MAIN_T_JS_ZHONGZHI(MAIN_T);
				//处理
			DEAL_CHULI(dataDealFlg);//响应请求		
			LED_RCV(0); 
			disPlay_Clear(1,11);//清屏显示
				//判断处理结果	
			if(rcvArr[(2+rcvArr[1])] == jiaoYanHe((rcvArr+4),(rcvArr[1]-2)))//||((GONGNENG_CHAXUN(Gongneng)==QINGLING)&&(startFlg<5))||((GONGNENG_CHAXUN(Gongneng)==BTLJYW)&&(startFlg>10)))//判断校验和是否正确  if(rcvArr[(2+rcvArr[1])] == jiaoYanHe((rcvArr+4),(rcvArr[1]-2)))  ===
			{
				switch(GONGNENG_CHAXUN(Gongneng))
				{
					case BIAOHAO:
					case XISHU:
					case QINGLING:
					case KOUJING:
						if(rcvArr[1]==0x0A)						
						{
							successFlg=1;
						}
						break;
				}
			}
			else
			{
				successFlg=0;
			}
			
			if(successFlg)
			{
				disPlay_Clear(1,11);//清屏显示
				//DEAL_RES_OK(dataDealFlg);//成功标志
				SEG[2]=SHUZHI[5];//S
				SEG[3]=(SHUZHI[0]&(~A));//U
				SEG[4]=A+F+E+D;//C
				SEG[5]=A+F+E+D;//C
				SEG[6]=A+F+G+E+D;//E
				SEG[7]=SHUZHI[5];//S
				SEG[8]=SHUZHI[5];//S
			}
			else
			{
				SEG[3]=SHUZHI[0x0E];// 显示Error
				SEG[4]=E+G;
				SEG[5]=E+G;
				SEG[6]=E+G+C+D;
				SEG[7]=E+G;
			}					
			xianShi();			
			successFlg=0;//接收正确标志位初始化
			
			//LPUart_ClrStatus(LPUART1,LPUartRC);
			if(sendWay==HONGWAI)
			{
				Uart_EnableFunc(UARTCH1,UartRx);//处理完后打开接收发送功能
			}
			else
			{
				LPUart_EnableFunc(LPUART1,LPUartRx);//处理完后打开接收发送功能
			}
			
			lpowerFlg=1;//只要接受后就可进入低功耗
			
		}
			
//超时处理		
		if(MAIN_T_CS_CHAXUN(MAIN_T))//发送后判断从板回复数据是否超时
		{
			MAIN_T_CS_QING(MAIN_T);//MAIN_T_JS_ZHONGZHI(MAIN_T);//去除超时标志位
			disPlay_Clear(1,11);//清屏显示	
			
			if(sendWay==HONGWAI)//红外功能主要用于联创改表号 自动＋1
			{
				LED_RCV(0);
			}
			else//以下为有线发送方式
			{			
			
			if(((GONGNENG_CHAXUN(Gongneng)==QINGLING)&&(startFlg<5))||((GONGNENG_CHAXUN(Gongneng)==BTLJYW)&&(((M0P_GPIO->PCOUT)&0x0400)!=0)))
			{
				//DEAL_RES_OK(dataDealFlg);//成功标志
				SEG[2]=SHUZHI[5];//S
				SEG[3]=(SHUZHI[0]&(~A));//U
				SEG[4]=A+F+E+D;//C
				SEG[5]=A+F+E+D;//C
				SEG[6]=A+F+G+E+D;//E
				SEG[7]=SHUZHI[5];//S
				SEG[8]=SHUZHI[5];//S
			}
			else
			{
				SEG[3]=SHUZHI[0x0E];// 显示Error
				SEG[4]=E+G;
				SEG[5]=E+G;
				SEG[6]=E+G+C+D;
				SEG[7]=E+G;
			}
			
			if((startFlg==0)&&((M0P_GPIO->PCOUT)&0x0400)==0)
			{
				SEG[9]|= H_LCD;//显示h -> 表示通讯超时	
			}
			
			LED_RCV(0);
			xianShi();
		}
		lpowerFlg=1;//只要接受后就可进入低功耗
		}
		//是否需要切换波特率
		if(sBtl==1)
		{
			sBtl=0;
			swpBtl();
		}
#endif
		Wdt_Feed();
		
//		if(((currentBtl&0xF0)!=0x10)&&(sendWay!=HONGWAI)&&(lpowerFlg==1))//9600时和用红外发送时都不能进入低功耗
		if(lpowerFlg==1)//9600时和用红外发送时都不能进入低功耗
		{
			Lpm_GotoLpmMode();
		}	
		//JIANZHI = 0xFF;
	}
//	return 0;
}

