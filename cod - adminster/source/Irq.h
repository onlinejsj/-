
#ifndef __IRQ_H__
#define __IRQ_H__


#include "init.h"




#define BTLJYW 0x0A 	//������(����ϵ��)
#define BIAOHAO 0x0B
#define JIANYI 0x0C
#define KOUJING 0x0D
#define QUEREN 0x0F		//����(ȷ�ϼ�)
#define XISHU 0x0E 
#define QINGLING 0x00 //����(����У��)


//#define AUTOBIAOHAO 0x09//ȷ�ϼ����ͺ����Զ�+1  ϵͳʱ�䰴����Ϊ���-1

#define YOUXIAN 0x01 //�����޸�
#define HONGWAI 0x02  //�����޸�




#define GONGNENG_JIANCHA_XIANGYING(x) ((x&0x10)||(x&0x80))  //�����Ӧ Gongneng��λ�Ƿ�Ϊ 1
#define GONGNENG_ZHIXING_XIANGYING(x) (x&=0xEF) //ִ������Ӧ Gongneng��λ��0
#define GONGNENG_QINGQIU_XIANGYING(x) (x|=0x10)  //������Ӧ  Gongneng��λ��1
#define GONGNENG_CHAXUN(x) (x&0x0F) //���ܲ�ѯ


#define MAIN_T_JISHI(x) (x|=0x10)//MAIN_T �е�4λΪ��ʱ��־λ����0-7λ��
#define MAIN_T_JS_ZHONGZHI(x) (x&=0xEF) //ֹͣ��ʱ
#define MAIN_T_JS_CHAXUN(x) (x&0x10)//��ѯ�Ƿ���Ҫ��ʱ
#define MAIN_T_CHAOSHI(x) (x|=0x20)//MAIN_T �е�5λΪ���ճ�ʱ��־λ
#define MAIN_T_CS_CHAXUN(x)  (x&0x20)//�鿴�Ƿ�ʱ
#define MAIN_T_CS_QING(x) (x&=0xDF)//ȥ����ʱ��־
#define OUT_TIME 0x0Au;//5s

#define DEAL_QINGQIU(x) x|=0x01  //��������յ�����  ��1λΪ�������־
#define DEAL_QQ_CHAXUN(x) x&0x01  // ��ѯ�Ƿ��д������־
#define DEAL_CHULI(x) x&=0xFE //��Ӧ��������յ����� ����������־
//#define DEAL_RES_OK(x) x|=0x02; //���ճɹ�  ��2λΪ�ɹ���־λ  �ɹ���1
//#define DEAL_RES_CHAXUN(x) x&=0x02; //��������ѯ
//#define DEAL_RES_CLEAR(x) x&=0xFD;//���������  �ȴ��´δ�����


//������
#define N9600 0x10 //9600 ��
#define N2400 0x00 
#define E9600 0x12 
#define E2400 0x02 // 2400 ż
//�ھ�
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

#define XIPING_T 600 //600==5����


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
extern uint8_t sendWay;//1 Lp����  2 UART����(����)


extern uint8_t currentBtl;
extern uint8_t lpowerFlg;


void rtcIrqFct(void);
void RcvFct(void);
void RcvFct_UARTCH1(void);






//irq   1






#endif

