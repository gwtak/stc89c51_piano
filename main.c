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
unsigned char n=0;  //nΪ���ĳ������� 
//���֡����¹𻨡�����ʽΪ: Ƶ�ʳ���, ���ĳ���, Ƶ�ʳ���, ���ĳ���,
unsigned char code music_tab[] ={   
0x18, 0x30, 0x1C , 0x10,  
0x20, 0x40, 0x1C , 0x10,   
0x18, 0x10, 0x20 , 0x10,   
0x1C, 0x10, 0x18 , 0x40,   
0x1C, 0x20, 0x20 , 0x20,   
0x1C, 0x20, 0x18 , 0x20,   
0x20, 0x80, 0x00    
};  


//����timer0�ж�0���ƽ��� 
void timer0()  interrupt 1      
{  TH0=0xd8;   
   TL0=0xef;   
   n--;   
}   
   
//����Ƶ����ʱ    
void delay (unsigned char m)   
{   
 unsigned i=3*m;   
 while(--i);   
}   

//������ʱ�ӳ���   
void delayms(unsigned char a)      
{   
  while(--a);											//����while(--a) ��Ҫ����while(a--);
}   
  
//��������ɨ�裬���KeyValue��Χ0~15  
void KeyDown(void)
{
	char a=0;
	GPIO_KEY=0x0f;//�����ƽ������λ�͵�ƽ������λ�ߵ�ƽ
	if(GPIO_KEY!=0x0f)//��ȡ�����Ƿ���
	{
		delay(5);//��ʱ����
		if(GPIO_KEY!=0x0f)//�ٴμ������Ƿ���
		{	
			//������
			GPIO_KEY=0X0F;//�����ƽ������λ�͵�ƽ������λ�ߵ�ƽ 	
			switch(GPIO_KEY)
			{
				case(0X07):	KeyValue=0;break;
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;
			}
			//������
			GPIO_KEY=0XF0;// �����ƽ������λ�ߵ�ƽ������λ�͵�ƽ
			switch(GPIO_KEY)
			{
				case(0X70):	KeyValue=KeyValue;break;
				case(0Xb0):	KeyValue=KeyValue+4;break;
				case(0Xd0): KeyValue=KeyValue+8;break;
				case(0Xe0):	KeyValue=KeyValue+12;break;
			}
			while((a<50)&&(GPIO_KEY!=0xf0))	 //��ⰴ�����ּ��
			{
				delay(5);
				a++;
			}
		}
	}
}

//��led��
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

//�ر�led��
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

//���Ŷ�Ӧ��Ŀ
void playmusic(void)
{
	char m,p,i=0;
	TMOD=0x01; //��ʱ��0����01��ʽ 
	TH0=0xd8;TL0=0xef;   
	IE=0x82;   
play:   
   while(1)   
    {
		char pm[]={0x13,0x18,0x1C,0x20,0x30,0x40,0x80};
		KeyDown();
		//�����������
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
		//��Ŀ���¹�
		if (KeyValue==7)
			{
				a: 	p=music_tab[i];   
					if(p==0x00)       { i=0, delayms(1000); goto play;}     //�������������,��ʱ1��,�ص���ʼ����һ��    
					else if(p==0xff)  { i=i+1;delayms(100),TR0=0; goto a;}  //��������ֹ��,��ʱ100ms,����ȡ��һ����    
					else         	  {m=music_tab[i++], n=music_tab[i++];} //ȡƵ�ʳ��� �� ���ĳ���    
					TR0=1;                                             		//����ʱ��1    
					while(n!=0) Beep=~Beep,LED(m),delay(m),off(m);     		//�ȴ��������, ͨ��P1�������Ƶ
					TR0=0;                                             		//�ض�ʱ��1    
			}
	}
}



void main()   
{ 
	D8=0;
	playmusic();
}  