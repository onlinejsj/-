
#ifndef __IRQ_H__
#define __IRQ_H__


#include "init.h"




#define BTLJYW 0x0A 	//波特率(右上系数)
#define BIAOHAO 0x0B
#define JIANYI 0x0C
#define KOUJING 0x0D
#define QUEREN 0x0F		//发送(确认键)
#define XISHU 0x0E 
#define QINGLING 0x00 //清零(抄表校对)


//#define AUTOBIAOHAO 0x09//确认键发送后表号自动+1  系统时间按键变为表号-1

#define YOUXIAN 0x01 //有线修改
#define HONGWAI 0x02  //红外修改




#define GONGNENG_JIANCHA_XIANGYING(x) ((x&0x10)||(x&0x80))  //检测相应 Gongneng高位是否为 1
#define GONGNENG_ZHIXING_XIANGYING(x) (x&=0xEF) //执行完响应 Gongneng高位置0
#define GONGNENG_QINGQIU_XIANGYING(x) (x|=0x10)  //请求响应  Gongneng高位置1
#define GONGNENG_CHAXUN(x) (x&0x0F) //功能查询


#define MAIN_T_JISHI(x) (x|=0x10)//MAIN_T 中第4位为计时标志位（共0-7位）
#define MAIN_T_JS_ZHONGZHI(x) (x&=0xEF) //停止计时
#define MAIN_T_JS_CHAXUN(x) (x&0x10)//查询是否需要计时
#define MAIN_T_CHAOSHI(x) (x|=0x20)//MAIN_T 中第5位为接收超时标志位
#define MAIN_T_CS_CHAXUN(x)  (x&0x20)//查看是否超时
#define MAIN_T_CS_QING(x) (x&=0xDF)//去除超时标志
#define OUT_TIME 0x0Au;//5s

#define DEAL_QINGQIU(x) x|=0x01  //请求处理接收的数据  第1位为待处理标志
#define DEAL_QQ_CHAXUN(x) x&0x01  // 查询是否有待处理标志
#define DEAL_CHULI(x) x&=0xFE //相应请求处理接收的数据 清除待处理标志
//#define DEAL_RES_OK(x) x|=0x02; //接收成功  第2位为成功标志位  成功置1
//#define DEAL_RES_CHAXUN(x) x&=0x02; //处理结果查询
//#define DEAL_RES_CLEAR(x) x&=0xFD;//处理结果清空  等待下次处理结果


//波特率
#define N9600 0x10 //9600 无
#define N2400 0x00 
#define E9600 0x12 
#define E2400 0x02 // 2400 偶
//口径
#define DN15 0x01
#define DN20 0x02
#define DN25 0x03
#define DN32 0x04
#define DN40 0x05
#define DN50 0x06
#define DN65 0x07
#define DN80 0x08
#define DN100 0x09
#define DN125 0x0A
#define DN150 0x0B
#define DN200 0x0C
#define DN250 0x0D
#define DN300 0x0E
#define DN350 0x0F
#define DN400 0x10
#define DN450 0x11
#define DN500 0x12

#define DN550 0x13
#define DN600 0x14
#define DN700 0x15


#define ADDR_LAST_BTLJYW 0x8000
#define ADDR_LAST_Ver 0x8001
#define ADDR_LAST_xs5 0x8002
#define ADDR_LAST_xs7 0x8004
#define ADDR_LAST_xs9 0x8006
#define ADDR_LAST_xs6 0x8008
#define ADDR_LAST_xs8 0x800A
#define ADDR_LAST_ID 0x800C
#define ADDR_LAST_KJ 0x8010
#define ADDR_LAST_XSD 0x8012

#define LED_SEND(x) M0P_GPIO->PCOUT_f.PC11=x
#define LED_RCV(x) M0P_GPIO->PCOUT_f.PC10=x
//#define LED_RCV_CHAXUN M0P_GPIO->PCOUT_f.PC10
#define LED_RESULT(x) M0P_GPIO->PAOUT_f.PA15=x
#define LED_POWER(x) M0P_GPIO->PCOUT_f.PC12=x
#define EN_LH(x) M0P_GPIO->PDOUT_f.PD04=x

#define XIPING_T 600 //600==5分钟


extern uint8_t rcvArr[36];
extern uint8_t MAIN_T;
extern uint8_t JIANZHI;
extern uint32_t Jianzhihecheng;
extern uint8_t Gongneng ;
extern uint8_t Xishuhao;
//extern uint8_t koujinghao;
extern uint8_t btl_jyw;
extern uint8_t startFlg;
extern uint8_t dataDealFlg;

extern int16_t xpTime;

extern uint8_t sBtl;
extern uint8_t sendWay;//1 Lp发送  2 UART发送(红外)


extern uint8_t currentBtl;
extern uint8_t lpowerFlg;


void rtcIrqFct(void);
void RcvFct(void);
void RcvFct_UARTCH1(void);






//irq   1






#endif

