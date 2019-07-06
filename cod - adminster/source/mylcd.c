

#include "mylcd.h"

void userXianShi(void)
{
//	if(GONGNENG_CHAXUN(Gongneng)!=JIANYI)
//	{
		disPlay_Clear(1,11);//先清屏
//	}
	switch(GONGNENG_CHAXUN(Gongneng))
	{			
			case BTLJYW:
				switch(lastTimeData.btlJyw)
				{
					case N9600:
					case E9600:
						SEG[2]=SHUZHI[9];
						SEG[3]=SHUZHI[6];
						SEG[4]=SHUZHI[0];
						SEG[5]=SHUZHI[0];
						break;
					case N2400:						
					case E2400:
						SEG[2]=SHUZHI[2];
						SEG[3]=SHUZHI[4];
						SEG[4]=SHUZHI[0];
						SEG[5]=SHUZHI[0];
						break;
					default:
						lastTimeData.btlJyw=E2400;
						SEG[2]=SHUZHI[2];
						SEG[3]=SHUZHI[4];
						SEG[4]=SHUZHI[0];
						SEG[5]=SHUZHI[0];
						break;
				}
				if(((lastTimeData.btlJyw)&0x0F) == 0x00)
				{
					SEG[7]=E+F+A+B+C;//N
				}
				else
				{
					SEG[7]=A+F+G+E+D;//E
				}
				break;
			case BIAOHAO:
				valueDisplay(lastTimeData.ID,8,0);//保存的为十进制  发送时需编程相同的BCD码
				break;
			//case SHIJIAN:
			case KOUJING:
				SEG[3]=SHUZHI[0x0D];//显示 dn
				SEG[4]=E+G+C;
				switch(lastTimeData.kj)//
				{
					case 15://15
					case 20:
					case 25:
					case 32:
					case 40:
					case 50:
					case 65:
					case 80:
					case 100:
					case 125:
					case 150:
					case 200:
					case 250:
					case 300:
					case 350:
					case 400:
					case 450:
					case 500:
					case 550:
					case 600:
					case 700:
						valueDisplay(lastTimeData.kj,8,1);
						break;
					default:
						SEG[6] = SHUZHI[0x0E];
						SEG[7] = E+G;
						SEG[8] = E+G;
						break;
				}
				break;
			case XISHU:
				switch(Xishuhao)
				{
					case 1:
						SEG[1]=SHUZHI[5];
						valueDisplay(lastTimeData.xs5,8,0);
						break;
					case 2:
						SEG[1]=SHUZHI[7];
						valueDisplay(lastTimeData.xs7,8,0);
						break;
					case 3:
						SEG[1]=SHUZHI[9];
						valueDisplay(lastTimeData.xs9,8,0);
						break;
					case 4:
						SEG[1]=SHUZHI[6];
						valueDisplay(lastTimeData.xs6,8,0);
						break;
					case 5:
						SEG[1]=SHUZHI[8];
						valueDisplay(lastTimeData.xs8,8,0);
						break;
				}
				break;
			case QINGLING:
				valueDisplay(0,8,8);
				break;
			default:
				break;		
	}
	xianShi();
}



/***************************************************
* function： 输入数值及要求自动显示数值
* value：    要显示的数值
* start_P：  从哪个位置开始显示，注意是最低位
* actNum：   最小有效位
****************************************************/
void valueDisplay(uint32_t value,uint8_t start_P,int8_t actNum)
{	
	int32_t temp=value;
	
	if((start_P>8)||(start_P<1)||(temp<0))
	{
		return;
	}
	while((temp>0)||(actNum>0))
	{
		SEG[start_P]=SHUZHI[(temp%10)];
		temp = temp/10;
		if(start_P>0)
			start_P--;
		actNum--;
	}
	xianShi();
}

/***************************************************
* function： 输入数值及要求自动显示数值
* value：    要显示的数值
* start_P：  从哪个位置开始显示，注意是最低位
* actNum：   最小有效位
****************************************************/
void valueDisplay1(uint32_t value,uint8_t start_P,int8_t actNum,uint8_t JZ)
{	
	uint32_t temp=value;
	
	if((start_P>8)||(start_P<1)||(temp==0))
	{
		return;
	}
	while((temp>0)||(actNum>0))
	{
		SEG[start_P]=SHUZHI[(temp%JZ)];
		temp = temp/JZ;
		if(start_P>0)
			start_P--;
		actNum--;
	}
	xianShi();
}



/*
					SEG[2]=SHUZHI[5];//S
					SEG[3]=(SHUZHI[0]&(~A));//U
					SEG[4]=A+F+E+D;//C
					SEG[5]=A+F+E+D;//C
					SEG[6]=A+F+G+E+D;//E
					SEG[7]=SHUZHI[5];//S
					SEG[8]=SHUZHI[5];//S
*/


/***************************************************
* function： 输入十进制生成BCD码
* value：    十进制
****************************************************/
uint32_t DEC_BCD(uint32_t value)
{
	return (value+((value/10)*6));
}

/***************************************************
* function： BCD码输入十进制生成
* value：    十进制
****************************************************/
uint32_t BCD_DEC(uint32_t value)
{
	return (value-((value>>4)*6));
}

/***************************************************
* function： 显示清空
****************************************************/
void disPlay_Clear(uint8_t c_start,uint8_t len)
{
	uint8_t i=0;
	if((c_start+len)<13)
	{
		for(i=0;i<len;i++)
		{
			SEG[(c_start+i)]=0;
		}
	}
}

