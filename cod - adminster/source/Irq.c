
#include "irq.h"
#include "ANjian.h"
#include "mylcd.h"

#define ANJIAN_IN 			((((M0P_GPIO->PBIN)&(0x3C0))>>4)&(0xFF))




uint8_t statuMachine=0;  //状态机_状态
uint8_t delFlag;
uint8_t MAIN_T=0;       //总的循环周期计数器
uint8_t JIANZHI = 0xFF; //初始化为错误键值 正等键值正常后说明有输入(main函数判断有按键按下使用)
uint32_t Jianzhihecheng=0;//单个数合成正常数值
uint8_t Gongneng;//功能
uint8_t Xishuhao=5; //系数号 579
//uint8_t koujinghao=0;
uint8_t btl_jyw; //波特率校验位 00：2400-无  02:2400-偶  10：9600-无   12：9600-偶
uint8_t startFlg;
uint8_t dataDealFlg;
uint8_t sBtl=0;
int16_t xpTime=XIPING_T;//上电初始值为5min

uint8_t fuc_model_times;//一字节 高位运行模式  低位长按计数
uint8_t xishu_duan;//一字节 高位系数个数  低位长按计数
uint8_t sendWay;

uint8_t currentBtl;
uint8_t lpowerFlg=1;

uint8_t JianZhi[4][4] = {
	{0x07,0x08,0x09,0x0A},	
	{0x04,0x05,0x06,0x0B},
	{0x01,0x02,0x03,0x0C},
	{0x00,0x0D,0x0E,0x0F}
};

uint8_t rcvArr[36]={0};


/*------- 周期中断 -------*/
void rtcIrqFct(void)
{
	static uint8_t flg=0;
	static uint8_t t=0;
		
	
	if(MAIN_T_JS_CHAXUN(MAIN_T))//查询是否需要计时
	{
		MAIN_T--;
		if((MAIN_T&0x0F)==0)
		{
			MAIN_T_CHAOSHI(MAIN_T);
			MAIN_T_JS_ZHONGZHI(MAIN_T);
		}
	}
	//长按查询
	if(ANJIAN_IN==0x38)//判断确认键那列是否被按下
	{
		EnableNvic(PORTB_IRQn,IrqLevel3,FALSE);
		M0P_GPIO->PDOUT_f.PD02=1;
		if( (ANJIAN_IN) == 0x3C )
		{
			fuc_model_times++;
			if((fuc_model_times&0x0F)>=3)
			{
				MAIN_T_CS_QING(MAIN_T);//计时停止并初始化
				MAIN_T_JS_ZHONGZHI(MAIN_T);
				disPlay_Clear(1,8);
				fuc_model_times &= 0xF0;
				JIN_XUANZHEMOSHI(fuc_model_times);
				SEG[1]=SHUZHI[0x0F];
				SEG[2]=SHUZHI[sendWay];//显示当前通讯方式 
				lpowerFlg=1;
				if(GONGNENG_CHAXUN(Gongneng)==XISHU)
				{
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
				}
				valueDisplay(GONGNENG_CHAXUN(Gongneng),8,0);//显示当前功能
//				xianShi();
			}
		}
		else if(ANJIAN_IN==0x38)
		{
		}
		
		M0P_GPIO->PDOUT_f.PD02=0;			
		M0P_GPIO->PB_ICLR = 0;//清中断标志			
		EnableNvic(PORTB_IRQn,IrqLevel3,TRUE);
	}
	else if((fuc_model_times&0x0F)!=0)//避免多次按确认键正好记录3次而误进入功能选择模式
	{
		fuc_model_times &= 0xF0;
	}
	
	
	//息屏计时
	if(xpTime>0)
	{
		//LED_POWER((flg&0x01));
		xpTime--;
		if(xpTime==0)
		{
			LCD_EnFunc(LcdEn,FALSE);
			flg=0;
			
			Gongneng=0;//3.15以后添加
			saveLastTimeData();			
			lastTimeData.btlJyw=E2400;//息屏后改为低波特率  这样主循环会进入低功耗，  唤醒后会自动读取一次flash中保存的状态（下同）
		  sendWay=YOUXIAN;//只有设置成有线才会进入低功耗
			
			Sysctrl_SetPeripheralGate(SysctrlPeripheralLcd,FALSE);			
			Sysctrl_SetPeripheralGate(SysctrlPeripheralLpUart1,FALSE);
			Sysctrl_ClkSourceEnable(SysctrlClkRCL,FALSE);
		
			LED_POWER(0);
			
		}
		//valueDisplay(xpTime,8,1);
		
		t++;
		if(t==5)
		{
			LED_POWER((flg&0x01));
			t=0;
			//flg=~flg;
		}	
		else if(t==1)
		{
			LED_POWER(0);
		}
		flg=~flg;	
	}
	
}



/*------- LPuart串口接收中断 -----*/
void RcvFct(void)
{
	uint8_t rcvData=0;
	rcvData = M0P_LPUART1->SBUF_f.DATA;
	LED_RCV(1);
	if((startFlg==0)&&(rcvData!=0x68))
	{
		return;
	}
	rcvArr[startFlg] = rcvData;
	LPUart_ClrStatus(LPUART1,LPUartRC);
	startFlg++;	
	
	if(((startFlg-4)>=rcvArr[1])&&(rcvData==0x16))//if((startFlg-4)==rcvArr[1])
	{
		LPUart_DisableFunc(LPUART1,LPUartRx);//字节收取完成  关闭接收功能，主程序处理完后 接收功能再次打开
		DEAL_QINGQIU(dataDealFlg);
		return;
	}
	
	LED_RCV(0);	
	
	if(startFlg==150)
	{
		startFlg--;
	}
}

/*------- UARTCH1串口接收中断 -----*/
void RcvFct_UARTCH1(void)
{
	uint8_t rcvData=0;
	rcvData = M0P_UART1->SBUF_f.DATA;
	LED_RCV(1);
	if((startFlg==0)&&(rcvData!=0x68))
	{
		return;
	}
	rcvArr[startFlg] = rcvData;
	Uart_ClrStatus(UARTCH1,UartRC);
	startFlg++;	
	
	if(((startFlg-4)>=rcvArr[1])&&(rcvData==0x16))//if((startFlg-4)==rcvArr[1])
	{
		Uart_DisableFunc(UARTCH1,UartRx);//字节收取完成  关闭接收功能，主程序处理完后 接收功能再次打开
		DEAL_QINGQIU(dataDealFlg);
		return;
	}
	
	LED_RCV(0);	
	
	if(startFlg==150)
	{
		startFlg--;
	}
}



/*------------- 按键中断读数 ----------
 *配合init.c中按键端口初始化管脚配置中断
 *----------------------------------*/
void Gpio_IRQHandler(uint8_t u8Param)
{
	volatile uint8_t x=5;//初始化数值不在数组内作为出错判断依据
	volatile uint8_t y=5;
	uint8_t readVal=0;

	if(xpTime==0)
	{
		Sysctrl_SetPeripheralGate(SysctrlPeripheralLcd,TRUE);
		Sysctrl_SetPeripheralGate(SysctrlPeripheralLpUart1,TRUE);
		Sysctrl_ClkSourceEnable(SysctrlClkRCL,TRUE);
		readLastTimeData();//3.15后添加
		swpBtl();
		
		LCD_EnFunc(LcdEn,TRUE);//唤醒屏幕
		
		LED_POWER(1);		
	}
	xpTime=XIPING_T;//5分钟
	
	
	readVal=ANJIAN_IN;
	
	//3.8修改处
	M0P_GPIO->PB_ICLR = 0;//清中断标志
	
	if(1==u8Param)//0~3代表A~B端口，此处选择端口B
	{
		delay1ms(20);//消抖
		if( (ANJIAN_IN) != readVal )//判断是不是按键抖动
		{
			return ;
		}		
//		JIANZHI = 0xFF;
		switch(	readVal )
		{
		case 0x1C:
			y=0;
			break;
		case 0x2C:			
			y=1;
			break;
		case 0x34:
			y=2;
			break;	
		case 0x38:
			y=3;
			break;	
		}
		if(y<4)
		{
			M0P_GPIO->PDOUT_f.PD02=1;
			readVal=ANJIAN_IN;
			if( (ANJIAN_IN) == 0x3C )
			{
				x=3;
			}
			else
			{
				M0P_GPIO->PBOUT_f.PB03=1;
			readVal=ANJIAN_IN;
				if( (ANJIAN_IN) == 0x3C )//(ANJIAN_IN) != readVal
				{
					x=2;
				}
				else
				{
					M0P_GPIO->PBOUT_f.PB04=1;
			readVal=ANJIAN_IN;
					if( (ANJIAN_IN) == 0x3C )
					{
						x=1;
					}
					else
					{
						M0P_GPIO->PBOUT_f.PB05=1;
			readVal=ANJIAN_IN;
						if( (ANJIAN_IN) == 0x3C )
						{
							x=0;
						}
						else
						{
							;//  每一行都不是
						}
					}
				}
			}
			
	EnableNvic(PORTB_IRQn,IrqLevel3,FALSE);
			M0P_GPIO->PDOUT_f.PD02=0;
			M0P_GPIO->PBOUT_f.PB03=0;
			M0P_GPIO->PBOUT_f.PB04=0;
			M0P_GPIO->PBOUT_f.PB05=0;			
	M0P_GPIO->PB_ICLR = 0;//清中断标志			
	EnableNvic(PORTB_IRQn,IrqLevel3,TRUE);
			
			JIANZHI = JianZhi[x][y];
		}
		else
			
		{			
			x=5;//恢复初始值
			y=5;
			JIANZHI = 0xFF;
		}
				
		if( (JIANZHI>0x09)&&(JIANZHI<0x10) )//不是 数字键
		{
			if((GONGNENG_CHAXUN(Gongneng)==0x0A)&&(JIANZHI!=0x0A)&&(JIANZHI!=0x0F))//切换非波特率菜单，且上次菜单是 “波特率”和发送按键
			{
				sBtl=1;
				/*
				切换波特率
				*/
//				swpBtl();  //下同
//				LPUart_DisableFunc(LPUART1,LPUartRx);
//				switch(lastTimeData.btlJyw)
//				{
//					case N9600:
//						LPUart_SetMode(LPUART1,LPUartMode1);
//						LPUart_SetBaud(LPUART1,1);
//						break;
//					case N2400:
//						LPUart_SetMode(LPUART1,LPUartMode1);
//						LPUart_SetBaud(LPUART1,4);
//						break;
//					case E9600:
//						LPUart_SetMode(LPUART1,LPUartMode3);
//						LPUart_SetBaud(LPUART1,1);
//						LPUart_SetMMDOrCk(LPUART1,LPUartEven);
//						break;
//					case E2400:
//						LPUart_SetMode(LPUART1,LPUartMode3);
//						LPUart_SetBaud(LPUART1,4);
//						LPUart_SetMMDOrCk(LPUART1,LPUartEven);
//						break;					
//				}
//				LPUart_EnableFunc(LPUART1,LPUartRx);
			}
			delay10us(1);
			if(JIANZHI==0x0F)
			{
				Gongneng |= 0x80;
			}
			else
			{
				if(((GONGNENG_CHAXUN(JIANZHI))==JIANYI)&&(sendWay==HONGWAI)&&((GONGNENG_CHAXUN(Gongneng))==BIAOHAO))//此功能主要用于联创改表号使用
				{
					lastTimeData.ID--;
					valueDisplay(lastTimeData.ID,8,0);
				}
				else if((GONGNENG_CHAXUN(JIANZHI))!=JIANYI)//正常处理（只要不在功能选项中选择2 时间按键没有功能）
				{
				Gongneng = GONGNENG_QINGQIU_XIANGYING(JIANZHI);
				}
			}
		}
	}
	
}

