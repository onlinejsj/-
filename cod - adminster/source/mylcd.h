
#ifndef __LCD_MY_H__
#define __LCD_MY_H__

#include "init.h"
#include "irq.h"



void userXianShi(void);//��ѭ�������ʾ���⺯����ʶ���ֲ�������ʾ---��
void valueDisplay(uint32_t value,uint8_t start_P,int8_t actNum);//��ֵ�Զ���ʾ������������ֵ�Զ���ʾ����
void valueDisplay1(uint32_t value,uint8_t start_P,int8_t actNum,uint8_t JZ);//��Ҫ������ƽ���


uint32_t BCD_DEC(uint32_t value);
uint32_t DEC_BCD(uint32_t value);
void disPlay_Clear(uint8_t c_start,uint8_t len);//��ʾ��������







#endif
