#include <REG52.H>    
#include <INTRINS.H> 
#define GPIO_KEY P1   
sbit D1=P2^0;
sbit D2=P2^1;
sbit D3=P2^2;
sbit D4=P2^3;
sbit D5=P2^4;
sbit D6=P2^5;
sbit D7=P2^6;
sbit D8=P2^7;
   

sbit Beep =  P1^5; 
int KeyValue=100;
unsigned char n=0;  //n为节拍常数变量 
//音乐《八月桂花》，格式为: 频率常数, 节拍常数, 频率常数, 节拍常数,
unsigned char code music_tab[] ={   
0x18, 0x30, 0x1C , 0x10,  
0x20, 0x40, 0x1C , 0x10,   
0x18, 0x10, 0x20 , 0x10,   
0x1C, 0x10, 0x18 , 0x40,   
0x1C, 0x20, 0x20 , 0x20,   
0x1C, 0x20, 0x18 , 0x20,   
0x20, 0x80, 0x00    
};  


//采用timer0中断0控制节拍 
void timer0()  interrupt 1      
{  TH0=0xd8;   
   TL0=0xef;   
   n--;   
}   
   
//控制频率延时    
void delay (unsigned char m)   
{   
 unsigned i=3*m;   
 while(--i);   
}   

//毫秒延时子程序   
void delayms(unsigned char a)      
{   
  while(--a);											//采用while(--a) 不要采用while(a--);
}   
  
//按键行列扫描，输出KeyValue范围0~15  
void KeyDown(void)
{
	char a=0;
	GPIO_KEY=0x0f;//输入电平，高四位低电平，低四位高电平
	if(GPIO_KEY!=0x0f)//读取按键是否按下
	{
		delay(5);//延时消抖
		if(GPIO_KEY!=0x0f)//再次检测键盘是否按下
		{	
			//测试列
			GPIO_KEY=0X0F;//输入电平，高四位低电平，低四位高电平 	
			switch(GPIO_KEY)
			{
				case(0X07):	KeyValue=0;break;
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;
			}
			//测试行
			GPIO_KEY=0XF0;// 输入电平，高四位高电平，低四位低电平
			switch(GPIO_KEY)
			{
				case(0X70):	KeyValue=KeyValue;break;
				case(0Xb0):	KeyValue=KeyValue+4;break;
				case(0Xd0): KeyValue=KeyValue+8;break;
				case(0Xe0):	KeyValue=KeyValue+12;break;
			}
			while((a<50)&&(GPIO_KEY!=0xf0))	 //检测按键松手检测
			{
				delay(5);
				a++;
			}
		}
	}
}

//打开led灯
void LED(char m)
{
	switch(m)
	{
		case(0x13):D1=0;break;
		case(0x18):D2=0;break;
		case(0x1c):D3=0;break;
		case(0x20):D4=0;break;
		case(0x30):D5=0;break;
		case(0x40):D6=0;break;
		case(0x80):D7=0;break;
	}
}

//关闭led灯
void off(char m)
{
	switch(m)
	{
		case(0x13):D1=1;break;
		case(0x18):D2=1;break;
		case(0x1c):D3=1;break;
		case(0x20):D4=1;break;
		case(0x30):D5=1;break;
		case(0x40):D6=1;break;
		case(0x80):D7=1;break;
	}
}

//播放对应曲目
void playmusic(void)
{
	char m,p,i=0;
	TMOD=0x01; //定时器0采用01方式 
	TH0=0xd8;TL0=0xef;   
	IE=0x82;   
play:   
   while(1)   
    {
		char pm[]={0x13,0x18,0x1C,0x20,0x30,0x40,0x80};
		KeyDown();
		//单个音阶输出
		if(KeyValue>=0&&KeyValue<7)
		{
			m=pm[KeyValue];
			n=0x10;
			TR0=1;
			while(n!=0)
			{
				Beep=~Beep;
				LED(m);
				delay(m);
				off(m);
			}
			TR0=0;
			KeyValue=100;
			break;
		}
		//曲目八月桂花
		if (KeyValue==7)
			{
				a: 	p=music_tab[i];   
					if(p==0x00)       { i=0, delayms(1000); goto play;}     //如果碰到结束符,延时1秒,回到开始再来一遍    
					else if(p==0xff)  { i=i+1;delayms(100),TR0=0; goto a;}  //若碰到休止符,延时100ms,继续取下一音符    
					else         	  {m=music_tab[i++], n=music_tab[i++];} //取频率常数 和 节拍常数    
					TR0=1;                                             		//开定时器1    
					while(n!=0) Beep=~Beep,LED(m),delay(m),off(m);     		//等待节拍完成, 通过P1口输出音频
					TR0=0;                                             		//关定时器1    
			}
	}
}



void main()   
{ 
	D8=0;
	playmusic();
}  