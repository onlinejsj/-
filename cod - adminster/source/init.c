
#include "init.h"
#include "Irq.h"


	uint16_t SHUZHI[16]={ 
		A+B+C+D+E+F,C+B,A+B+G+E+D,A+B+C+G+D,F+B+G+C,//0~5
		A+F+G+C+D,A+F+G+C+D+E,A+B+C,A+B+C+D+E+F+G,G+F+A+B+C+D,		//5~9
		A+F+E+B+C,F+E+G+C+D,A+F+E+D,G+E+D+B+C,A+F+G+E+D,A+F+E+G  //A~F
	};
	
	uint16_t SEG[12]={0};	//只显示数值为SEG[9]={0}
	
	/***************** 自封装 屏幕刷新显示函数 *****************/ 
void xianShi(void)  //
{
		uint8_t j=0;
		volatile uint32_t *ptemp = NULL;
    ptemp = (volatile uint32_t*)&M0P_LCD->RAM5;
		*ptemp = SEG[1];
		for(j=2;j<12;j++)//若只显示数值  应为j<9
		{
			ptemp--;
			*ptemp=SEG[j];
			*ptemp <<= 16;
			j++;
			*ptemp|=SEG[j];
		}
	}

/*****************系统时钟 初始化*****************/ 
void ClkInit(void)
{
	
	stc_sysctrl_clk_config_t sysclkConfig;
	sysclkConfig.enClkSrc = SysctrlClkRCH;
	sysclkConfig.enHClkDiv = SysctrlHclkDiv1;
	sysclkConfig.enPClkDiv = SysctrlPclkDiv1;
	Sysctrl_ClkInit(&sysclkConfig);
	
	Sysctrl_ClkSourceEnable(SysctrlClkXTL,TRUE);//使能外部晶振32768
	Sysctrl_ClkSourceEnable(SysctrlClkRCL,TRUE);
	Sysctrl_SetRCLTrim(SysctrlRclFreq38400);
	
}

/*****************UART设置初始化*****************/   //红外口
void Uart_portInit(void)  //初始化串口IO
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
	stc_uart_multimode_t multiMode; ///<多主机模式配置
  stc_uart_irq_cb_t  pstcIrqCb;	///<中断服务函数配置  	
	stc_uart_baud_t baudStr;
	
	uint16_t baudCal;
	
	DDL_ZERO_STRUCT(uartConfig);
	DDL_ZERO_STRUCT(multiMode);
	DDL_ZERO_STRUCT(pstcIrqCb);
	DDL_ZERO_STRUCT(baudStr);
	
	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralUart1,TRUE);
	//配置工作模式
	uartConfig.enRunMode = UartMode3;//模式3可选择校验位波特率    model1只能选择合适的波特率
	uartConfig.enStopBit = Uart1bit;
	
	
	multiMode.enMulti_mode = UartNormal;
	pstcIrqCb.pfnRxIrqCb = RcvFct_UARTCH1 ;//串口接收中断函数
	
	
	uartConfig.pstcMultiMode = &multiMode;
	uartConfig.pstcIrqCb = &pstcIrqCb;
	
	
	Uart_ClrStatus(UARTCH1,UartRC);//清标志位
	uartConfig.bTouchNvic = TRUE;
	
	
	Uart_Init(UARTCH1,&uartConfig);
	
	
	Uart_EnableIrq(UARTCH1,UartRxIrq);	//打开指定中断功能   
	Uart_EnableFunc(UARTCH1,UartRx); //打开串口指定功能        
  Uart_SetMMDOrCk(UARTCH1,UartEven);//设置校验位
	//设置波特率
	Uart_SetClkDiv(UARTCH1,Uart16Or32Div); // 不设置亦可
	baudStr.enRunMode = UartMode3;
	baudStr.u16Baud = 2400;
	baudStr.u32Pclk = Sysctrl_GetPClkFreq();
	baudCal = Uart_CalScnt(UARTCH1,&baudStr);
	Uart_SetBaud(UARTCH1,baudCal);	
}
  

/*****************  LP_UART设置初始化  *****************/
void LP_Uart_portInit(void)  //初始化串口IO
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
  
/*****************按键GPIO设置初始化*****************/
void initIo(void)
{
	stc_gpio_config_t GpioConfig;

	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);	
	
	DDL_ZERO_STRUCT(GpioConfig);//清10
	GpioConfig.enDir = GpioDirIn;
	GpioConfig.enPuPd = GpioPu;
	Gpio_Init(GpioPortB,GpioPin6,&GpioConfig);
	Gpio_Init(GpioPortB,GpioPin7,&GpioConfig);
	Gpio_Init(GpioPortB,GpioPin8,&GpioConfig);
	Gpio_Init(GpioPortB,GpioPin9,&GpioConfig);
	M0P_GPIO->PBFIE |= 0x03C0;  //开下降沿中断
	
	DDL_ZERO_STRUCT(GpioConfig);//清	
	GpioConfig.enDir = GpioDirOut;
	Gpio_Init(GpioPortD,GpioPin2,&GpioConfig);
	Gpio_Init(GpioPortB,GpioPin3,&GpioConfig);
	Gpio_Init(GpioPortB,GpioPin4,&GpioConfig);
	Gpio_Init(GpioPortB,GpioPin5,&GpioConfig);
	M0P_GPIO->PDOUT_f.PD02 = 0;//初始化输出低电平 下同
	M0P_GPIO->PBOUT &= ~(0x38);
	
	//指示灯
	Gpio_Init(GpioPortC,GpioPin10,&GpioConfig);//接收
	LED_RCV(0);
	Gpio_Init(GpioPortC,GpioPin11,&GpioConfig);//发送
	LED_SEND(0);
	Gpio_Init(GpioPortC,GpioPin12,&GpioConfig);//电源
	LED_POWER(0);
	Gpio_Init(GpioPortA,GpioPin15,&GpioConfig);//结果
	LED_RESULT(0);
	Gpio_Init(GpioPortD,GpioPin4,&GpioConfig);//EN管脚
	EN_LH(0);

}


/*****************RTC设置初始化*****************/
void rtcInit(void)
{
	stc_rtc_config_t rtcConfig;
	stc_rtc_cyc_sel_t rtcIrqTtimeConfig;
	stc_rtc_irq_cb_t rtcIrqUserConfig;
	stc_rtc_time_t stcTime;
	
	Sysctrl_ClkSourceEnable(SysctrlClkXTL,TRUE);//使能外部晶振32768
//	Sysctrl_ClkSourceEnable(SysctrlClkRCL,TRUE);//使能内部晶振38400 
//	Sysctrl_SetRCLTrim(SysctrlRclFreq38400);//预设为38400
	Sysctrl_SetPeripheralGate(SysctrlPeripheralRtc,TRUE);

	DDL_ZERO_STRUCT(rtcConfig);
	DDL_ZERO_STRUCT(rtcIrqTtimeConfig);
	DDL_ZERO_STRUCT(rtcIrqUserConfig);

	rtcConfig.enAmpmSel = Rtc24h;
	rtcConfig.enClkSel = RtcClk32768;//时钟选择 外部32768 RtcClk32768

	rtcIrqTtimeConfig.enCyc_sel=RtcPradx; //周期中断     RtcPrads = 0u, ///<月、天、时分秒
	//rtcIrqTtimeConfig.enPrds_sel=Rtc_None; //无PRDS中断周期  Rtc_05S
	rtcIrqTtimeConfig.u8Prdx=0; //0.5s
	rtcConfig.pstcCycSel = &rtcIrqTtimeConfig;

	rtcIrqUserConfig.pfnTimerIrqCb = rtcIrqFct; //周期中断函数
	rtcConfig.pstcIrqCb = &rtcIrqUserConfig;

	//日期设置
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

	Rtc_DisableFunc(RtcCount);//先禁用
	Rtc_Init(&rtcConfig);//RTC初始化设置
	EnableNvic(RTC_IRQn,IrqLevel3,TRUE);
}

/**************** WDT设置初始化 ***************************/
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

/**************** Lpm设置初始化 ***************************/
void lpmInit(void)
{
	stc_lpm_config_t lpmConfig;
	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralFlash,TRUE);          ///////flash外设使能
	
	lpmConfig.enSEVONPEND = SevPndEnable;
	lpmConfig.enSLEEPDEEP = SlpDpEnable;
	lpmConfig.enSLEEPONEXIT = SlpExtDisable;
	Lpm_Config(&lpmConfig);
}

/**************** Lpt设置初始化 ***************************/
void lptInit(void)
{
}



/**************** Lpt设置初始化 ***************************/
void lcdInit(void)
{
	stc_lcd_config_t lcdConfig;
	Sysctrl_SetPeripheralGate(SysctrlPeripheralLcd,TRUE);
	//配置相关串口为模拟模式
	M0P_GPIO->PAADS |= 0x1EFC;//1E为com口
	M0P_GPIO->PBADS |= 0xFC07;
	M0P_GPIO->PCADS |= 0x3F0;
	M0P_LCD->POEN0 &= 0xFFC00001;//配置SEG端口
	M0P_LCD->POEN1 &= 0x000010FF;//配置COM端口
	
	lcdConfig.bTouchNvic = FALSE;
	lcdConfig.enBias = LcdBias3;
	lcdConfig.enBiasSrc = LcdInRes_Low;//LcdInRes_Mid
	lcdConfig.enClk = LcdRCL;//LcdXTL
	lcdConfig.enCpClk = LcdClk2k;//电压泵时钟频率配置
	lcdConfig.enDispMode = LcdMode0;
	lcdConfig.enDuty = LcdDuty4;
	lcdConfig.enScanClk = LcdClk128hz; //扫描频率配置64时屏幕闪烁  
	
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
	


