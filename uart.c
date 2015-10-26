
#include "REG52.h"
#define uchar unsigned  char
#define uint  unsigned   int  

void UART_ISR(void);
void UARTSnd(unsigned char dat);
void UART_init();
void MeterTempByUART();
/*******************************************************************************************/
sbit     dula  = P2^6;         //显示段选锁存控制端
sbit     wela  = P2^7;         //显示位选锁存控制端

uchar     dispbuf[4];            //显示缓冲区
uchar idata GLengthLoh[2];    //接收缓冲区 内部可编程256位ram


/************************共阴数码管显示位码+数码管0-9段码**********************************/
uchar code dispbit[]={/* 0xfe,0xfd,0xfb,0xf7, */0xef,0xdf,0xbf,0x7f};//共阴极数码管显示位码
/************************************数码管0-9段码****************************************/
uchar code SEG7[]= {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};//共阴极数码管0-9 -
/*******************************************************************************************/
void UART_ISR(void) interrupt 4
{
    uchar temp ,k;
    ES = 0;         //  关中断            
    if(RI)
    {
        RI = 0;         // 清接收标志
        temp = SBUF;
        GLengthLoh[k] = temp;
        k++;
        if(k == 2) //  k = 数据长度
        k = 0;
    }    
    ES = 1;      //开中
}
/*******************************************************************************************/
void UARTSnd(unsigned char dat)//向串口发送一个字符
{
    TI = 0;            // 清发送标志
    SBUF = dat;        //如果TI为0等待
    while (!TI);    // wait until sent
}
/*******************************************************************************************/
void UART_init()
{    
    TH1 = 0xFD;
    TL1 = 0xFD;
    TMOD = 0x21;            //设置定时器1为模式2;定时器0为模式1      
    TH0 =(65536-2500)/256; //约2.50MS定时器初值
    TL0 =(65536-2500)%256; //约2.50MS定时器初值
    ET0 = 1;
    TR0 = 1;                //启动定时器
    TR1 = 1;                //启动定时器        SM0  SM1
    SM0 = 0;                //串口通信模式设置     0      0 = 0
    SM1 = 1;                                //     0      1 = 1
                                            //     1      0 = 2
                                            //   1       1 = 3
    REN = 1;     //串口允许接收数据
     ES = 1;        //开串中断
    EA = 1;
}
/*******************************************************************************************/
//定时器0中断,用做显示
timer0() interrupt 1  // 定时器0中断是1号
{
    uchar dispcount;  //自定义寄存器
    static unsigned char count=0;
    TH0 =(65536-2500)/256; //约2.50MS定时器初值
    TL0 =(65536-2500)%256; //约2.50MS定时器初值    TH0 = 0xef;    TL0 = 0x94;    //写入定时器0初始值
    dula = 0; dula = 1;
    P0 = SEG7[dispbuf[dispcount]];    //根据dispcount当前计算值到dispbuf
                                    //显示缓冲存储区取待显示值作为参数到
                                    //SEG7段码存储区取显示段码赋给P0口    
    dula = 0; wela = 0; wela = 1;
    P0 = dispbit[dispcount];    //根据dispcount当前计算值到dispbit位选存储区
                                //取显示位码赋给P0口
    wela = 0;    
    dispcount++;
    if(dispcount == 4)            //共4位显示位
    {
        dispcount = 0;
    }
    count++;
    if(count == 100)        //约25MS读取一次
    {
        count = 0;
        MeterTempByUART();
    }        
}
/*******************************************************************************************/
void DisplayLength(unsigned long number)
{
    if(number > 4500)        //如果大于4.5米，显示 "-"
    {
        dispbuf[0] = 10;    //显示 "-"
        dispbuf[1] = 10;    //显示 "-"
        dispbuf[2] = 10;    //显示 "-"
        dispbuf[3] = 10;    //显示 "-"
    }
    else if(number < 4500)
    {
        dispbuf[0] = number / 1000;
        dispbuf[1] = number % 1000 / 100;
        dispbuf[2] = number % 100 / 10;
        dispbuf[3] = number % 10;
    }
    
}    
/*******************************************************************************************/
void MeterTempByUART()
{
    UARTSnd(0X55);
}
/*******************************************************************************************/
void main(void)
{
    unsigned long PreLength = 0;
    UART_init();
    while(1)
    {        
        PreLength = GLengthLoh[0] * 256 + GLengthLoh[1];    //距离高八位和距离低八位
        DisplayLength(PreLength);    
        //DisplayLength(1234);    
    }
}
