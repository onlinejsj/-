
#include "init.h"
#include "Irq.h"


	uint16_t SHUZHI[16]={ 
		A+B+C+D+E+F,C+B,A+B+G+E+D,A+B+C+G+D,F+B+G+C,//0~5
		A+F+G+C+D,A+F+G+C+D+E,A+B+C,A+B+C+D+E+F+G,G+F+A+B+C+D,		//5~9
		A+F+E+B+C,F+E+G+C+D,A+F+E+D,G+E+D+B+C,A+F+G+E+D,A+F+E+G  //A~F
	};
	
	uint16_t SEG[12]={0};	//ֻ��ʾ��ֵΪSEG[9]={0}
	
	/***************** �Է�װ ��Ļˢ����ʾ���� *****************/ 
void xianShi(void)  //
{
		uint8_t j=0;
		volatile uint32_t *ptemp = NULL;
    ptemp = (volatile uint32_t*)&M0P_LCD->RAM5;
		*ptemp = SEG[1];
		for(j=2;j<12;j++)//��ֻ��ʾ��ֵ  ӦΪj<9
		{
			ptemp--;
			*ptemp=SEG[j];
			*ptemp <<= 16;
			j++;
			*ptemp|=SEG[j];
		}
	}

/*****************ϵͳʱ�� ��ʼ��*****************/ 
void ClkInit(void)
{
	
	stc_sysctrl_clk_config_t sysclkConfig;
	sysclkConfig.enClkSrc = SysctrlClkRCH;
	sysclkConfig.enHClkDiv = SysctrlHclkDiv1;
	sysclkConfig.enPClkDiv = SysctrlPclkDiv1;
	Sysctrl_ClkInit(&sysclkConfig);
	
	Sysctrl_ClkSourceEnable(SysctrlClkXTL,TRUE);//ʹ���ⲿ����32768
	Sysctrl_ClkSourceEnable(SysctrlClkRCL,TRUE);
	Sysctrl_SetRCLTrim(SysctrlRclFreq38400);
	
}

/*****************UART���ó�ʼ��*****************/   //�����
void Uart_portInit(void)  //��ʼ������IO
{
	stc_gpio_config_t gpioConfig;
	DDL_ZERO_STRUCT(gpioConfig);

	gpioConfig.enDir = GpioDirIn; //Rx
	Gpio_Init(GpioPortD,GpioPin1,&gpioConfig);
	Gpio_SetAfMode(GpioPortD,GpioPin1,GpioAf3);
	DDL_ZERO_STRUCT(gpioConfig);
	gpioConfig.enDir = GpioDirOut;//Tx
	Gpio_Init(GpioPortD,GpioPin0,&gpioConfig);
	Gpio_SetAfMode(GpioPortD,GpioPin0,GpioAf3);
}


void Uart_configInit(void)
{
	stc_uart_config_t uartConfig;
	stc_uart_multimode_t multiMode; ///<������ģʽ����
  stc_uart_irq_cb_t  pstcIrqCb;	///<�жϷ���������  	
	stc_uart_baud_t baudStr;
	
	uint16_t baudCal;
	
	DDL_ZERO_STRUCT(uartConfig);
	DDL_ZERO_STRUCT(multiMode);
	DDL_ZERO_STRUCT(pstcIrqCb);
	DDL_ZERO_STRUCT(baudStr);
	
	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralUart1,TRUE);
	//���ù���ģʽ
	uartConfig.enRunMode = UartMode3;//ģʽ3��ѡ��У��λ������    model1ֻ��ѡ����ʵĲ�����
	uartConfig.enStopBit = Uart1bit;
	
	
	multiMode.enMulti_mode = UartNormal;
	pstcIrqCb.pfnRxIrqCb = RcvFct_UARTCH1 ;//���ڽ����жϺ���
	
	
	uartConfig.pstcMultiMode = &multiMode;
	uartConfig.pstcIrqCb = &pstcIrqCb;
	
	
	Uart_ClrStatus(UARTCH1,UartRC);//���־λ
	uartConfig.bTouchNvic = TRUE;
	
	
	Uart_Init(UARTCH1,&uartConfig);
	
	
	Uart_EnableIrq(UARTCH1,UartRxIrq);	//��ָ���жϹ���   
	Uart_EnableFunc(UARTCH1,UartRx); //�򿪴���ָ������        
  Uart_SetMMDOrCk(UARTCH1,UartEven);//����У��λ
	//���ò�����
	Uart_SetClkDiv(UARTCH1,Uart16Or32Div); // ���������
	baudStr.enRunMode = UartMode3;
	baudStr.u16Baud = 2400;
	baudStr.u32Pclk = Sysctrl_GetPClkFreq();
	baudCal = Uart_CalScnt(UARTCH1,&baudStr);
	Uart_SetBaud(UARTCH1,baudCal);	
}
  

/*****************  LP_UART���ó�ʼ��  *****************/
void LP_Uart_portInit(void)  //��ʼ������IO
{
		stc_gpio_config_t stcGpioCfg;
			
    stcGpioCfg.enDir =  GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin0,&stcGpioCfg);//TX
    stcGpioCfg.enDir =  GpioDirIn;
    Gpio_Init(GpioPortA,GpioPin1,&stcGpioCfg);//RX
    
    Gpio_SetAfMode(GpioPortA,GpioPin0,GpioAf2);
    Gpio_SetAfMode(GpioPortA,GpioPin1,GpioAf2);
    
}

void LP_Uart_configInit(void)
{
		uint16_t u16Scnt=0;
	  stc_gpio_config_t stcGpioCfg;
    stc_lpuart_sclk_sel_t stcSclk;
    stc_lpuart_config_t  stcConfig;
    stc_lpuart_irq_cb_t stcLPUartIrqCb;
    stc_lpuart_multimode_t stcMulti;
    stc_lpuart_baud_t stcBaud;

    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcLPUartIrqCb);
    DDL_ZERO_STRUCT(stcMulti);
    DDL_ZERO_STRUCT(stcBaud);
    DDL_ZERO_STRUCT(stcGpioCfg);
    DDL_ZERO_STRUCT(stcSclk);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralLpUart1,TRUE);
	
    stcLPUartIrqCb.pfnRxIrqCb   = RcvFct;
    stcConfig.pstcIrqCb = &stcLPUartIrqCb;
    stcConfig.bTouchNvic = TRUE;
  
    stcConfig.enStopBit = LPUart1bit;
    stcConfig.enRunMode = LPUartMode1;//
    stcSclk.enSclk_Prs = LPUart4Or8Div;
    stcSclk.enSclk_sel = LPUart_Rcl;//LPUart_Pclk
    stcConfig.pstcLpuart_clk = &stcSclk;  
    

    stcMulti.enMulti_mode = LPUartNormal;
    stcConfig.pstcMultiMode = &stcMulti;
    
//    LPUart_SetMMDOrCk(LPUART1,LPUartEven);
    LPUart_Init(LPUART1, &stcConfig);
    
    LPUart_SetClkDiv(LPUART1,LPUart4Or8Div);
    
    stcBaud.u32Sclk = LPUart_GetSclk(LPUART1);
    stcBaud.enRunMode = LPUartMode1;
    stcBaud.u16Baud = 9600;
    u16Scnt = LPUart_CalScnt(LPUART1,&stcBaud);
    LPUart_SetBaud(LPUART1,u16Scnt);//
    
    LPUart_EnableIrq(LPUART1,LPUartRxIrq);
    LPUart_ClrStatus(LPUART1,LPUartRC);
    LPUart_EnableFunc(LPUART1,LPUartRx);
}
  
/*****************����GPIO���ó�ʼ��*****************/
void initIo(void)
{
	stc_gpio_config_t GpioConfig;

	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);	
	
	DDL_ZERO_STRUCT(GpioConfig);//��10
	GpioConfig.enDir = GpioDirIn;
	GpioConfig.enPuPd = GpioPu;
	Gpio_Init(GpioPortB,GpioPin6,&GpioConfig);
	Gpio_Init(GpioPortB,GpioPin7,&GpioConfig);
	Gpio_Init(GpioPortB,GpioPin8,&GpioConfig);
	Gpio_Init(GpioPortB,GpioPin9,&GpioConfig);
	M0P_GPIO->PBFIE |= 0x03C0;  //���½����ж�
	
	DDL_ZERO_STRUCT(GpioConfig);//��	
	GpioConfig.enDir = GpioDirOut;
	Gpio_Init(GpioPortD,GpioPin2,&GpioConfig);
	Gpio_Init(GpioPortB,GpioPin3,&GpioConfig);
	Gpio_Init(GpioPortB,GpioPin4,&GpioConfig);
	Gpio_Init(GpioPortB,GpioPin5,&GpioConfig);
	M0P_GPIO->PDOUT_f.PD02 = 0;//��ʼ������͵�ƽ ��ͬ
	M0P_GPIO->PBOUT &= ~(0x38);
	
	//ָʾ��
	Gpio_Init(GpioPortC,GpioPin10,&GpioConfig);//����
	LED_RCV(0);
	Gpio_Init(GpioPortC,GpioPin11,&GpioConfig);//����
	LED_SEND(0);
	Gpio_Init(GpioPortC,GpioPin12,&GpioConfig);//��Դ
	LED_POWER(0);
	Gpio_Init(GpioPortA,GpioPin15,&GpioConfig);//���
	LED_RESULT(0);
	Gpio_Init(GpioPortD,GpioPin4,&GpioConfig);//EN�ܽ�
	EN_LH(0);

}


/*****************RTC���ó�ʼ��*****************/
void rtcInit(void)
{
	stc_rtc_config_t rtcConfig;
	stc_rtc_cyc_sel_t rtcIrqTtimeConfig;
	stc_rtc_irq_cb_t rtcIrqUserConfig;
	stc_rtc_time_t stcTime;
	
	Sysctrl_ClkSourceEnable(SysctrlClkXTL,TRUE);//ʹ���ⲿ����32768
//	Sysctrl_ClkSourceEnable(SysctrlClkRCL,TRUE);//ʹ���ڲ�����38400 
//	Sysctrl_SetRCLTrim(SysctrlRclFreq38400);//Ԥ��Ϊ38400
	Sysctrl_SetPeripheralGate(SysctrlPeripheralRtc,TRUE);

	DDL_ZERO_STRUCT(rtcConfig);
	DDL_ZERO_STRUCT(rtcIrqTtimeConfig);
	DDL_ZERO_STRUCT(rtcIrqUserConfig);

	rtcConfig.enAmpmSel = Rtc24h;
	rtcConfig.enClkSel = RtcClk32768;//ʱ��ѡ�� �ⲿ32768 RtcClk32768

	rtcIrqTtimeConfig.enCyc_sel=RtcPradx; //�����ж�     RtcPrads = 0u, ///<�¡��졢ʱ����
	//rtcIrqTtimeConfig.enPrds_sel=Rtc_None; //��PRDS�ж�����  Rtc_05S
	rtcIrqTtimeConfig.u8Prdx=0; //0.5s
	rtcConfig.pstcCycSel = &rtcIrqTtimeConfig;

	rtcIrqUserConfig.pfnTimerIrqCb = rtcIrqFct; //�����жϺ���
	rtcConfig.pstcIrqCb = &rtcIrqUserConfig;

	//��������
	stcTime.u8Year = 0x18;
	stcTime.u8Month = 0x04;
	stcTime.u8Day = 0x16;
	stcTime.u8Hour = 0x10;
	stcTime.u8Minute = 0x58;
	stcTime.u8Second = 0x55;
	stcTime.u8DayOfWeek = Rtc_CalWeek(&stcTime.u8Day);
	rtcConfig.pstcTimeDate = &stcTime;

	Rtc_ClrIrqStatus(RtcPrdf);
	rtcConfig.bTouchNvic = TRUE;

	Rtc_DisableFunc(RtcCount);//�Ƚ���
	Rtc_Init(&rtcConfig);//RTC��ʼ������
	EnableNvic(RTC_IRQn,IrqLevel3,TRUE);
}

/**************** WDT���ó�ʼ�� ***************************/
void wdtInit(void)
{
	stc_wdt_config_t wdtConfig;
	DDL_ZERO_STRUCT(wdtConfig);
	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralWdt,TRUE);
	wdtConfig.enResetEnable = WRESET_EN;
	wdtConfig.pfnWdtIrqCb = NULL;
	wdtConfig.u8LoadValue = 0x0C;//6.55s
	
	Wdt_Init(&wdtConfig);
}

/**************** Lpm���ó�ʼ�� ***************************/
void lpmInit(void)
{
	stc_lpm_config_t lpmConfig;
	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralFlash,TRUE);          ///////flash����ʹ��
	
	lpmConfig.enSEVONPEND = SevPndEnable;
	lpmConfig.enSLEEPDEEP = SlpDpEnable;
	lpmConfig.enSLEEPONEXIT = SlpExtDisable;
	Lpm_Config(&lpmConfig);
}

/**************** Lpt���ó�ʼ�� ***************************/
void lptInit(void)
{
}



/**************** Lpt���ó�ʼ�� ***************************/
void lcdInit(void)
{
	stc_lcd_config_t lcdConfig;
	Sysctrl_SetPeripheralGate(SysctrlPeripheralLcd,TRUE);
	//������ش���Ϊģ��ģʽ
	M0P_GPIO->PAADS |= 0x1EFC;//1EΪcom��
	M0P_GPIO->PBADS |= 0xFC07;
	M0P_GPIO->PCADS |= 0x3F0;
	M0P_LCD->POEN0 &= 0xFFC00001;//����SEG�˿�
	M0P_LCD->POEN1 &= 0x000010FF;//����COM�˿�
	
	lcdConfig.bTouchNvic = FALSE;
	lcdConfig.enBias = LcdBias3;
	lcdConfig.enBiasSrc = LcdInRes_Low;//LcdInRes_Mid
	lcdConfig.enClk = LcdRCL;//LcdXTL
	lcdConfig.enCpClk = LcdClk2k;//��ѹ��ʱ��Ƶ������
	lcdConfig.enDispMode = LcdMode0;
	lcdConfig.enDuty = LcdDuty4;
	lcdConfig.enScanClk = LcdClk128hz; //ɨ��Ƶ������64ʱ��Ļ��˸  
	
	LCD_Init(&lcdConfig);
	LCD_EnFunc(LcdEn,TRUE);
	LCD_ClearDisp();	

//	SEG[3]=SHUZHI[1];
//	SEG[5]=SHUZHI[5];	
//	SEG[7]=SHUZHI[8];
//	SEG[10] |= DCBIAO_LCD;
//	SEG[11] |= RU_LCD;
//	SEG[10] |= ML_LCD;	
//	xianShi();
	
//	M0P_LCD->RAM5=SHUZHI[8];
//	M0P_LCD->RAM4=SHUZHI[2];
//	M0P_LCD->RAM4=((M0P_LCD->RAM4)<<16)|SHUZHI[6];
//	M0P_LCD->RAM3=SHUZHI[4];
//	M0P_LCD->RAM3=((M0P_LCD->RAM3)<<16)|SHUZHI[5];
//	M0P_LCD->RAM2=SHUZHI[6];
//	M0P_LCD->RAM2=((M0P_LCD->RAM2)<<16)|SHUZHI[7];
//	M0P_LCD->RAM1=((M0P_LCD->RAM1)|SHUZHI[8])<<16;
}
	


