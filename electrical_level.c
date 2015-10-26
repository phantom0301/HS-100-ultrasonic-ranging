#include <reg52.h>

#define uchar unsigned char 
#define uint  unsigned int

uchar code dofly_DuanMa[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};// 显示段码值0~9
uchar code dofly_WeiMa[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//分别对应相应的数码管点亮,即位码

uchar TempData[8]; //存储显示值的全局变量

sbit tr=P1^0;
sbit ec=P1^1;
sbit DUAN=P2^6;//定义锁存使能端口 段锁存
sbit WEI=P2^7;//                 位锁存

#define DataPort P0

uint distance,time,tl,th;

void delay_20us(void )
 { 
    uchar a ;
    for(a=0;a<100;a++);
 }

void delay(uint a )
 { 
   	while(a--);
 }

void Display(uchar FirstBit,uchar Num)
{
      uchar i;
	  
	  for(i=0;i<Num;i++)
	   { 
	    DataPort=0;   //清空数据，防止有交替重影
        DUAN=1;     //段锁存
        DUAN=0;

        DataPort=dofly_WeiMa[i+FirstBit]; //取位码 
        WEI=1;     //位锁存
        WEI=0;

        DataPort=TempData[i]; //取显示数据，段码
        DUAN=1;     //段锁存
        DUAN=0;
       
   	    delay(200); // 扫描间隙延时，时间太长会闪烁，太短会造成重影

       }

}

void main(void )
{	 
	
	TMOD=0x10;
	while(1)
	{
		tr=1;         //超声波输入端
	    delay_20us();   //延时20us
	    tr=0;         //产生一个20us的脉冲
	    while(ec==0); //等待Echo回波引脚变高电平 
	    TH1=0;          //定时器1清零
	    TL1=0;          //定时器1清零
	    TF1=0;          //计数溢出标志
	    TR1=1;          //启动定时器1
		 while(ec==1);
	  	 th=TH1;
		 tl=TL1;
		 TL1=0;
		 TH1=0;
	    TR1=0;          //关闭定时器1
	    time=th*256+tl;
	    distance=time*0.017;  //厘米		   */

		 TempData[0]=dofly_DuanMa[distance/100];//分解显示信息
	    TempData[1]=dofly_DuanMa[(distance%100)/10];
	    TempData[2]=dofly_DuanMa[(distance%100)%10];
       Display(4,3);

	}
}

