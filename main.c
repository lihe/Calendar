/*******************************************************************************
注意事项：
根据自己使用的LCD1602是否带有转接板，如果带有转接板的即为4位，需在LCD.H头文件中
将宏#define LCD1602_4PINS打开，我们这里使用的LCD1602是8位，所以默认将该宏注释。

接线说明：（开发攻略每个实验章节内的实验现象都有对应的接线图说明）
参考LCD1602液晶显示实验接线图
参考DS1302时钟实验接线图
K1-->P31
K2-->P30
K3-->P32
K4-->P33（该按键未做功能，大家可扩展）

实验操作：
LCD1602显示时钟，按K3键进入时钟设置，此时秒钟停止走动，按K1键选择设置的秒、分、时、
日、月、星期、年，按K2键进行加1，设置完成后，再次按下K3键继续走时
*******************************************************************************/

#include <reg51.h>
#include "lcd.h"
#include "ds1302.h"

sbit K1=P3^1;
sbit K2=P3^0;
sbit K3=P3^2;
sbit K4=P3^3;	 //管脚接线

void Int0Configuration();
void LcdDisplay();
unsigned char SetState,SetPlace;
void Delay10ms(void);   //误差 0us
/*******************************************************************************
* 函数名         : main
* 函数功能		   : 主函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/

void main()
{
	unsigned char i;

	Int0Configuration();
	LcdInit();
	Ds1302Init();
	while(1)
	{	
		if(SetState==0)
		{
			Ds1302ReadTime();
		}
		else
		{
			if(K1==0)		//检测按键K1是否按下
			{
				Delay10ms();	//消除抖动
				if(K1==0)
				{
					SetPlace++;
					if(SetPlace>=7)
						SetPlace=0;					
				}

				while((i<50)&&(K1==0))	 //检测按键是否松开
				{
					Delay10ms();
					i++;
				}
				i=0;
			}
			if(K2==0)		//检测按键K2是否按下
			{
				Delay10ms();	//消除抖动
				if(K2==0)
				{
					TIME[SetPlace]++;
					if((TIME[SetPlace]&0x0f)>9)					 //换成BCD码。
					{
						TIME[SetPlace]=TIME[SetPlace]+6;
					}
					if((TIME[SetPlace]>=0x60)&&(SetPlace<2))		//分秒只能到59
					{
						TIME[SetPlace]=0;
					}
					if((TIME[SetPlace]>=0x24)&&(SetPlace==2))		//小时只能到23
					{
						TIME[SetPlace]=0;
					}
					if((TIME[SetPlace]>=0x32)&&(SetPlace==3))		//日只能到31
					{
						TIME[SetPlace]=0;	
					}
					if((TIME[SetPlace]>=0x13)&&(SetPlace==4))		//月只能到12
					{
						TIME[SetPlace]=0;
					}	
					if((TIME[SetPlace]>=0x7)&&(SetPlace==5))		//周只能到7
					{
						TIME[SetPlace]=1;
					}		
//					if(SetPlace==5)		//月只能到12
//					{
//						TIME[SetPlace]=;
//					}	
				}
				
				while((i<50)&&(K2==0))	 //检测按键是否松开
				{
					Delay10ms();
					i++;
				}
				i=0;
				
			}						
		}
		LcdDisplay();	
	}
	
}
/*******************************************************************************
* 函数名         : LcdDisplay()
* 函数功能		   : 显示函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/

void LcdDisplay()
{
	LcdWriteCom(0x80+0X40);
	LcdWriteData('0'+TIME[2]/16);				//时
	LcdWriteData('0'+(TIME[2]&0x0f));				 
	LcdWriteData('-');
	LcdWriteData('0'+TIME[1]/16);				//分
	LcdWriteData('0'+(TIME[1]&0x0f));	
	LcdWriteData('-');
	LcdWriteData('0'+TIME[0]/16);				//秒
	LcdWriteData('0'+(TIME[0]&0x0f));

	LcdWriteCom(0x80);
	LcdWriteData('2');
	LcdWriteData('0');
	LcdWriteData('0'+TIME[6]/16);			//年
	LcdWriteData('0'+(TIME[6]&0x0f));
	LcdWriteData('-');
	LcdWriteData('0'+TIME[4]/16);			//月
	LcdWriteData('0'+(TIME[4]&0x0f));
	LcdWriteData('-');
	LcdWriteData('0'+TIME[3]/16);			//日
	LcdWriteData('0'+(TIME[3]&0x0f));
	LcdWriteCom(0x8D);
	LcdWriteData('0'+(TIME[5]&0x07));	//星期			 
}
/*******************************************************************************
* 函数名         : Int0Configuration()
* 函数功能		   : 配置外部中断0
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/

void Int0Configuration()
{
	//设置INT0
	IT0=1;//跳变沿出发方式（下降沿）
	EX0=1;//打开INT0的中断允许。
	EA=1;//打开总中断	
}
/*******************************************************************************
* 函数名         : Int0() 
* 函数功能		   : 外部中断0 中断函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/

void Int0() interrupt 0		 
{
	Delay10ms();
	if(K3==0)
	{
		SetState=~SetState;
		SetPlace=0;
		Ds1302Init();	
	}
}
/*******************************************************************************
* 函数名         : Delay10ms
* 函数功能		   : 延时函数，延时10ms
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void Delay10ms(void)   //误差 0us
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}