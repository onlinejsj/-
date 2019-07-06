
#include "irq.h"
#include "ANjian.h"
#include "mylcd.h"

#define ANJIAN_IN 			((((M0P_GPIO->PBIN)&(0x3C0))>>4)&(0xFF))




uint8_t statuMachine=0;  //״̬��_״̬
uint8_t delFlag;
uint8_t MAIN_T=0;       //�ܵ�ѭ�����ڼ�����
uint8_t JIANZHI = 0xFF; //��ʼ��Ϊ�����ֵ ���ȼ�ֵ������˵��������(main�����ж��а�������ʹ��)
uint32_t Jianzhihecheng=0;//�������ϳ�������ֵ
uint8_t Gongneng;//����
uint8_t Xishuhao=5; //ϵ���� 579
//uint8_t koujinghao=0;
uint8_t btl_jyw; //������У��λ 00��2400-��  02:2400-ż  10��9600-��   12��9600-ż
uint8_t startFlg;
uint8_t dataDealFlg;
uint8_t sBtl=0;
int16_t xpTime=XIPING_T;//�ϵ��ʼֵΪ5min

uint8_t fuc_model_times;//һ�ֽ� ��λ����ģʽ  ��λ��������
uint8_t xishu_duan;//һ�ֽ� ��λϵ������  ��λ��������
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


/*------- �����ж� -------*/
void rtcIrqFct(void)
{
	static uint8_t flg=0;
	static uint8_t t=0;
		
	
	if(MAIN_T_JS_CHAXUN(MAIN_T))//��ѯ�Ƿ���Ҫ��ʱ
	{
		MAIN_T--;
		if((MAIN_T&0x0F)==0)
		{
			MAIN_T_CHAOSHI(MAIN_T);
			MAIN_T_JS_ZHONGZHI(MAIN_T);
		}
	}
	//������ѯ
	if(ANJIAN_IN==0x38)//�ж�ȷ�ϼ������Ƿ񱻰���
	{
		EnableNvic(PORTB_IRQn,IrqLevel3,FALSE);
		M0P_GPIO->PDOUT_f.PD02=1;
		if( (ANJIAN_IN) == 0x3C )
		{
			fuc_model_times++;
			if((fuc_model_times&0x0F)>=3)
			{
				MAIN_T_CS_QING(MAIN_T);//��ʱֹͣ����ʼ��
				MAIN_T_JS_ZHONGZHI(MAIN_T);
				disPlay_Clear(1,8);
				fuc_model_times &= 0xF0;
				JIN_XUANZHEMOSHI(fuc_model_times);
				SEG[1]=SHUZHI[0x0F];
				SEG[2]=SHUZHI[sendWay];//��ʾ��ǰͨѶ��ʽ 
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
				valueDisplay(GONGNENG_CHAXUN(Gongneng),8,0);//��ʾ��ǰ����
//				xianShi();
			}
		}
		else if(ANJIAN_IN==0x38)
		{
		}
		
		M0P_GPIO->PDOUT_f.PD02=0;			
		M0P_GPIO->PB_ICLR = 0;//���жϱ�־			
		EnableNvic(PORTB_IRQn,IrqLevel3,TRUE);
	}
	else if((fuc_model_times&0x0F)!=0)//�����ΰ�ȷ�ϼ����ü�¼3�ζ�����빦��ѡ��ģʽ
	{
		fuc_model_times &= 0xF0;
	}
	
	
	//Ϣ����ʱ
	if(xpTime>0)
	{
		//LED_POWER((flg&0x01));
		xpTime--;
		if(xpTime==0)
		{
			LCD_EnFunc(LcdEn,FALSE);
			flg=0;
			
			Gongneng=0;//3.15�Ժ����
			saveLastTimeData();			
			lastTimeData.btlJyw=E2400;//Ϣ�����Ϊ�Ͳ�����  ������ѭ�������͹��ģ�  ���Ѻ���Զ���ȡһ��flash�б����״̬����ͬ��
		  sendWay=YOUXIAN;//ֻ�����ó����߲Ż����͹���
			
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



/*------- LPuart���ڽ����ж� -----*/
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
		LPUart_DisableFunc(LPUART1,LPUartRx);//�ֽ���ȡ���  �رս��չ��ܣ������������ ���չ����ٴδ�
		DEAL_QINGQIU(dataDealFlg);
		return;
	}
	
	LED_RCV(0);	
	
	if(startFlg==150)
	{
		startFlg--;
	}
}

/*------- UARTCH1���ڽ����ж� -----*/
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
		Uart_DisableFunc(UARTCH1,UartRx);//�ֽ���ȡ���  �رս��չ��ܣ������������ ���չ����ٴδ�
		DEAL_QINGQIU(dataDealFlg);
		return;
	}
	
	LED_RCV(0);	
	
	if(startFlg==150)
	{
		startFlg--;
	}
}



/*------------- �����ж϶��� ----------
 *���init.c�а����˿ڳ�ʼ���ܽ������ж�
 *----------------------------------*/
void Gpio_IRQHandler(uint8_t u8Param)
{
	volatile uint8_t x=5;//��ʼ����ֵ������������Ϊ�����ж�����
	volatile uint8_t y=5;
	uint8_t readVal=0;

	if(xpTime==0)
	{
		Sysctrl_SetPeripheralGate(SysctrlPeripheralLcd,TRUE);
		Sysctrl_SetPeripheralGate(SysctrlPeripheralLpUart1,TRUE);
		Sysctrl_ClkSourceEnable(SysctrlClkRCL,TRUE);
		readLastTimeData();//3.15�����
		swpBtl();
		
		LCD_EnFunc(LcdEn,TRUE);//������Ļ
		
		LED_POWER(1);		
	}
	xpTime=XIPING_T;//5����
	
	
	readVal=ANJIAN_IN;
	
	//3.8�޸Ĵ�
	M0P_GPIO->PB_ICLR = 0;//���жϱ�־
	
	if(1==u8Param)//0~3����A~B�˿ڣ��˴�ѡ��˿�B
	{
		delay1ms(20);//����
		if( (ANJIAN_IN) != readVal )//�ж��ǲ��ǰ�������
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
							;//  ÿһ�ж�����
						}
					}
				}
			}
			
	EnableNvic(PORTB_IRQn,IrqLevel3,FALSE);
			M0P_GPIO->PDOUT_f.PD02=0;
			M0P_GPIO->PBOUT_f.PB03=0;
			M0P_GPIO->PBOUT_f.PB04=0;
			M0P_GPIO->PBOUT_f.PB05=0;			
	M0P_GPIO->PB_ICLR = 0;//���жϱ�־			
	EnableNvic(PORTB_IRQn,IrqLevel3,TRUE);
			
			JIANZHI = JianZhi[x][y];
		}
		else
			
		{			
			x=5;//�ָ���ʼֵ
			y=5;
			JIANZHI = 0xFF;
		}
				
		if( (JIANZHI>0x09)&&(JIANZHI<0x10) )//���� ���ּ�
		{
			if((GONGNENG_CHAXUN(Gongneng)==0x0A)&&(JIANZHI!=0x0A)&&(JIANZHI!=0x0F))//�л��ǲ����ʲ˵������ϴβ˵��� �������ʡ��ͷ��Ͱ���
			{
				sBtl=1;
				/*
				�л�������
				*/
//				swpBtl();  //��ͬ
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
				if(((GONGNENG_CHAXUN(JIANZHI))==JIANYI)&&(sendWay==HONGWAI)&&((GONGNENG_CHAXUN(Gongneng))==BIAOHAO))//�˹�����Ҫ���������ı��ʹ��
				{
					lastTimeData.ID--;
					valueDisplay(lastTimeData.ID,8,0);
				}
				else if((GONGNENG_CHAXUN(JIANZHI))!=JIANYI)//��������ֻҪ���ڹ���ѡ����ѡ��2 ʱ�䰴��û�й��ܣ�
				{
				Gongneng = GONGNENG_QINGQIU_XIANGYING(JIANZHI);
				}
			}
		}
	}
	
}

