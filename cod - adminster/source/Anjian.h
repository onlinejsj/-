

	
	
#ifndef __ANJIAN_H__
#define __ANJIAN_H__

#include "irq.h"


extern uint8_t GaihaoArr[25];
extern uint8_t QinglingArr[40];
extern uint8_t GaiBtlArr[20];
extern uint8_t GaiXishuArr[30];

extern uint8_t QiangZhiArr[22];



	







void shengCheng_Send(void);
void gongNengAnJianChuLi(void);


void  uartSend_user_lp(uint8_t *arr,uint8_t length);
void uartSend_user(uint8_t *arr,uint8_t length);
en_result_t rcvArrDeal(void);

void readLastTimeData(void);	
void saveLastTimeData(void);
void swpBtl(void);	
	

void readAnJian_Hecheng(void);
uint8_t jiaoYanHe(uint8_t *arr,uint8_t length);














#endif

