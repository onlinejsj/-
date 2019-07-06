 
 #ifndef __INIT_H__
 #define __INIT_H__
 
 
 
	#include "ddl.h"
	#include "uart.h"
	#include "gpio.h"
	#include "rtc.h"
	#include "wdt.h"
	#include "lpm.h"
	#include "lpt.h"
	#include "lpuart.h"
	#include "flash.h"
	#include "sysctrl.h"
	#include "lcd.h"

	//以下为LCD字段宏定义
	#define A 0x08
	#define B 0x04
	#define C 0x02
	#define D 0x01
	#define E 0x0200
	#define G 0x0400
	#define F 0x0800	
	#define ZD_LCD 0x0100 //特殊字段：检定1  模拟2  历史3   瞬时4  热量5	 
		// SEG9
	#define H_LCD 0x0200 // /h
	#define V_LCD 0x0400 // m3
	#define T_LCD 0x0800 // ℃
	#define K_LCD 0x02 // km3中的k
	#define M_LCD 0x04 // MJ中的M
	#define WH_LCD 0x08//Wh
		// SEG10
	#define G_LCD 0x0200 // G
	#define ML_LCD 0x0400 // ML
	#define J_LCD 0x0800 // J	
	#define DCBIAO_LCD 0x01 // 电池图标
	#define FG_LCD 0x02 // 阀关
	#define FK_LCD 0x04 // 阀开
	#define WC_LCD 0x08 //温差
		// SEG11
	#define LL_LCD 0x0100 //冷量
	#define LIUL_LCD 0x0200 //流量
	#define RU_LCD 0x0400 //入口
	#define CHU_LCD 0x0800 //出口
	
	
	
	#define MOSHICHAXUN(x) x&0xF0
	#define ZHENGCHANGMOSHI 0x00 //正常模式
	#define JIN_ZHANGCHANGMOSHI(x) x&=0x0F	//进入正常模式	
	#define XUANZHEMOSHI 0x10 //功能选择模式
	#define JIN_XUANZHEMOSHI(x) x|=0x10;//进入功能选择模式 
	
	
	#define XISHU_3D 0x03//
	#define XISHU_4D 0x04//
	#define XISHU_5D 0x05// 

	#define QIANGPOSITION 15
	#define QIANGGUAN 0x99
	#define QIANGKAI 0x33
	
	 typedef struct flash_data
	 {
		 uint8_t btlJyw:8;
		 uint8_t Ver:8;//保存的是通讯方式红外还是有线方式
		 uint8_t xsd:8;//系数点34点系数还是5点系数
		 uint16_t xs5:16;
		 uint16_t xs7:16;
		 uint16_t xs9:16;
		 uint16_t xs6:16;
		 uint16_t xs8:16;
		 uint32_t ID:32;
		 uint16_t kj:16;
		 
	 }FLASH_DATA_INIT;
	
	extern FLASH_DATA_INIT lastTimeData;
 
 
	extern uint16_t SHUZHI[16];//0-9  SUCCESS N 
	extern uint16_t SEG[12];//若只显示数值为 SEG[9];
	extern uint8_t fuc_model_times;
	 
	extern uint8_t xishu_duan;
 
 
 //红外
	void Uart_portInit(void);
	void Uart_configInit(void);
	
 //485
	void LP_Uart_portInit(void);
	void LP_Uart_configInit(void);
	
	void ClkInit(void);
	void initIo(void);
	void rtcInit(void);
	void wdtInit(void);
	void lpmInit(void);
	void lcdInit(void);
	void lptInit(void);//未使用
	
		
	void xianShi(void);
	
		
	
	#endif
	
	

