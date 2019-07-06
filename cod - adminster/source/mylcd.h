
#ifndef __LCD_MY_H__
#define __LCD_MY_H__

#include "init.h"
#include "irq.h"



void userXianShi(void);//主循环里的显示调解函数，识别现操作并显示---总
void valueDisplay(uint32_t value,uint8_t start_P,int8_t actNum);//数值自动显示函数，输入数值自动显示函数
void valueDisplay1(uint32_t value,uint8_t start_P,int8_t actNum,uint8_t JZ);//需要输入进制进制


uint32_t BCD_DEC(uint32_t value);
uint32_t DEC_BCD(uint32_t value);
void disPlay_Clear(uint8_t c_start,uint8_t len);//显示数组清零







#endif
