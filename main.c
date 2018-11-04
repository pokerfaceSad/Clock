#include<reg51.h>

#define GPIO_DIG   P0	//段选
#define GPIO_PLACE P1	//位选
#define GPIO_KEY P2  //独立键盘用P2口
#define NOKEYPRESSED -1 //无按键按下
#define KEY1PRESSED 1 //按键1被按下
#define KEY2PRESSED 2 //按键2被按下
#define KEY3PRESSED 3 //按键3被按下
#define NOFLASH 0 //无闪烁
#define HOURFLASH 1 //小时数字闪烁
#define MINUTEFLASH 2 //小时数字闪烁
#define SECONDFLASH 3 //小时数字闪烁
#define ALLFLASH 4 //小时数字闪烁
#define NORMALSTATE 0 //时钟显示模式
#define TIMESETSTATE 1 //时间设置模式
#define ALARMSETSTATE 2 //闹钟设置模式

//--定义全局变量--//

//位选控制   查表的方法控制
unsigned char code DIG_PLACE[8] = {
0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
//0、1、2、3、4、5、6、7、8、9、A、b、C、d、E、F的显示码
unsigned char code DIG_CODE[17] = {
0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

sbit Beep =  P3^0 ; 
   
unsigned char n=0;  //n为节拍常数变量    
unsigned char code music_tab[] ={   
0x18, 0x30, 0x1C , 0x10, //格式为: 频率常数, 节拍常数, 频率常数, 节拍常数,    
0x20, 0x40, 0x1C , 0x10,   
0x18, 0x10, 0x20 , 0x10,   
0x1C, 0x10, 0x18 , 0x40,   
0x1C, 0x20, 0x20 , 0x20,   
0x1C, 0x20, 0x18 , 0x20,   
0x20, 0x80, 0xFF , 0x20,   
0x30, 0x1C, 0x10 , 0x18,   
0x20, 0x15, 0x20 , 0x1C,   
0x20, 0x20, 0x20 , 0x26,   
0x40, 0x20, 0x20 , 0x2B,   
0x20, 0x26, 0x20 , 0x20,   
0x20, 0x30, 0x80 , 0xFF,   
0x20, 0x20, 0x1C , 0x10,   
0x18, 0x10, 0x20 , 0x20,   
0x26, 0x20, 0x2B , 0x20,   
0x30, 0x20, 0x2B , 0x40,   
0x20, 0x20, 0x1C , 0x10,   
0x18, 0x10, 0x20 , 0x20,   
0x26, 0x20, 0x2B , 0x20,   
0x30, 0x20, 0x2B , 0x40,   
0x20, 0x30, 0x1C , 0x10,   
0x18, 0x20, 0x15 , 0x20,   
0x1C, 0x20, 0x20 , 0x20,   
0x26, 0x40, 0x20 , 0x20,   
0x2B, 0x20, 0x26 , 0x20,   
0x20, 0x20, 0x30 , 0x80,   
0x20, 0x30, 0x1C , 0x10,   
0x20, 0x10, 0x1C , 0x10,   
0x20, 0x20, 0x26 , 0x20,   
0x2B, 0x20, 0x30 , 0x20,   
0x2B, 0x40, 0x20 , 0x15,   
0x1F, 0x05, 0x20 , 0x10,   
0x1C, 0x10, 0x20 , 0x20,   
0x26, 0x20, 0x2B , 0x20,   
0x30, 0x20, 0x2B , 0x40,   
0x20, 0x30, 0x1C , 0x10,   
0x18, 0x20, 0x15 , 0x20,   
0x1C, 0x20, 0x20 , 0x20,   
0x26, 0x40, 0x20 , 0x20,   
0x2B, 0x20, 0x26 , 0x20,   
0x20, 0x20, 0x30 , 0x30,   
0x20, 0x30, 0x1C , 0x10,   
0x18, 0x40, 0x1C , 0x20,   
0x20, 0x20, 0x26 , 0x40,   
0x13, 0x60, 0x18 , 0x20,   
0x15, 0x40, 0x13 , 0x40,   
0x18, 0x80, 0x00   
};  

//用来存放要显示的8位数的值
unsigned char DisplayData[8];
//当前时间
unsigned char time_hour,time_minute,time_second;
//闹钟时间
unsigned char alarm_hour,alarm_minute,alarm_second;
//当前按键的值
int keyValue = NOKEYPRESSED;
//闪烁标志
int flashFlag = NOFLASH;
//工作状态标志
unsigned char stateFlag = NORMALSTATE;
//有效设置标志 
//如果用户在设置状态中进行了有效操作 则在状态转移时 仍回到原状态供用户查看设置结果
unsigned char setEffective = 0; 

//--声明全局函数--//
void DigDisplay(); //数码管显示函数
void delay(); //延时函数
void Init(); //初始化函数
void encode(); //解码函数
void UpdateTime(); //时间更新函数
void UpdateState(); //工作状态更新函数
void Key_Scan(); //按键检测函数
void Delayms(unsigned char a);
void NormalWork(); //时钟显示状态函数
void TimeSet();	//时间设置状态函数
void AlarmSet(); //闹钟设置函数
void Alarm(); //播放闹铃
void FrequencyDelay (unsigned char m);

void main(void)
{
	Init();
	encode();
	while(1)
	{
		//延时一秒
		delay();
		UpdateState();
		/*
			工作状态机
		*/
		switch(stateFlag){
	    	case NORMALSTATE:
			  	//时钟显示模式
				NormalWork();
				break;
			case TIMESETSTATE:
				//时间设置模式
				TimeSet();	
				break;
			case ALARMSETSTATE:
				//闹钟设置模式
				AlarmSet();
				break;
			
		
		}

	}				
}

/*
	初始化
*/
void Init(){

	//初始化计时器设置
	TH0 = (65536 - 50000)/256;
	TL0 = (65536 - 50000)%256;
	TMOD = 0x11;
	EA = 1;
	ET0 = 1;
	ET1 = 1;
	TR0 = 1;
	//初始化时间
	time_hour = 23;
	time_minute = 59;
	time_second = 50;
	//初始化闹钟
	alarm_hour = 20;
	alarm_minute = 13;
	alarm_second = 14;
}
/*
	时间更新函数 将当前时间全局变量增加1秒
*/
void UpdateTime(){
	time_second++;
	if(time_second == 60)
	{
		time_second = 0;
		time_minute++;
		if(time_minute == 60)
		{
			time_minute = 0;
			time_hour++;
			if(time_hour == 24)
			{
				time_hour = 0;
			}
		}
		
	}
			
		

}
/*
	工作状态更新函数 根据key1状态 更新工作状态全局变量
*/
void UpdateState(){

	//如果key1被按下 更新工作状态
	if(keyValue == KEY1PRESSED){
		//消费按键
		keyValue = NOKEYPRESSED;
		//更新工作状态标志
		if(setEffective){
			//如果进行了有效设置 则将有效设置标志重置 并回到原状态供用户查看设置结果
			setEffective = 0;
		 	stateFlag = stateFlag;
		} 
		else
			stateFlag = stateFlag == 2 ? NORMALSTATE : stateFlag+1; 
	}


}
/*
	解码函数 将int型数字解析为传递给数码管显示的段码 装入DisplayData
*/
void encode(){
	unsigned char hour,minute,second;
	unsigned char tens_place,ones_place;
	
	if(stateFlag == ALARMSETSTATE)
	{
		hour  = alarm_hour;
		minute = alarm_minute;
		second = alarm_second;	
	}
	else {
		hour = time_hour;
		minute = time_minute;
		second = time_second;
	}
	
	//小时十位
	tens_place = hour / 10; 
	//小时个位
	ones_place = (hour - (tens_place * 10));
	DisplayData[0] = DIG_CODE[tens_place];	  
	DisplayData[1] = DIG_CODE[ones_place];
	//分钟十位
	tens_place = minute / 10; 
	//分钟个位
	ones_place = (minute - (tens_place * 10));
	DisplayData[2] = DIG_CODE[tens_place];	  
	DisplayData[3] = DIG_CODE[ones_place];
	//秒钟十位
	tens_place = second / 10; 
	//秒钟个位
	ones_place = (second - (tens_place * 10));
	DisplayData[4] = DIG_CODE[tens_place];	  
	DisplayData[5] = DIG_CODE[ones_place];
	
	DisplayData[6] = 0x00;
	DisplayData[7] = 0x00;
}
/*
	数码管显示DisplayData
*/

void DigDisplay()
{
	unsigned char i;
	unsigned int j;
	for(i=0; i<8; i++)
	{
		if(flashFlag == ALLFLASH)
		{
			GPIO_PLACE = DIG_PLACE[i];
			GPIO_DIG = 0x00;
		}
		else if(i == (flashFlag)*2-2 || i == (flashFlag)*2-1)
		{
			GPIO_PLACE = DIG_PLACE[i];
			GPIO_DIG = 0x00;
		}
		else{
			GPIO_PLACE = DIG_PLACE[i];	 //发送位选
			GPIO_DIG = DisplayData[i];     //发送段码
			
		}
		j = 10;
		while(--j);
		GPIO_DIG = 0x00;//消隐
	}
	
}

/*	
	延时
*/
unsigned char num = 0; //定时器延时标志变量
unsigned char oneSecondEndFlag = 0;
void delay(){	
	//TR0 = 1;
	while(oneSecondEndFlag == 1 ? oneSecondEndFlag = 0,0:1){
		
		//显示到数码管
		DigDisplay();
		//Alarm();
		//更新闪烁标志
		if (GPIO_KEY != 0xFF)
		{
			Key_Scan();
			//这个break会导致时间不准确
			//break;
		}
											  
	}
	//num = 0;
	flashFlag = flashFlag > 0 ? flashFlag : -flashFlag;
	//TR0 = 0;
}

unsigned char timeSetFlag = 0;  //时间设置标志 依据此标志停止时间更新并初始化计时器
void T0_time() interrupt 1{
	TH0 = (65536 - 50000)/256;
	TL0 = (65536 - 50000)%256;
	num++;
	if(num == 10) flashFlag = -flashFlag;
	if(num == 20)
	{
		num = 0;
		oneSecondEndFlag = 1;
		
		if(stateFlag != TIMESETSTATE | setEffective != 1)
		{
			UpdateTime();
			encode();
		}
			
		
	}
}
/*
	播放闹铃
*/
unsigned char p,m,index=0;   //m为频率常数变量 
void Alarm(){
	//play:   
	 
	a: p=music_tab[index];   
	   if(p==0x00)       { index=0;}     //如果碰到结束符,延时1秒,回到开始再来一遍    
	   else if(p==0xff)  { index=index+1;Delayms(100),TR1=0; goto a;}  //若碰到休止符,延时100ms,继续取下一音符    
	   else         {m=music_tab[index++], n=music_tab[index++];}  //取频率常数 和 节拍常数    
	   TR1=1;                                             //开定时器1    
	   while(n!=0) Beep=~Beep,FrequencyDelay(m);
		                           //等待节拍完成, 通过P1口输出音频(可多声道哦!)    
	   TR1=0;                                             //关定时器1    
	


}
void int0()  interrupt 3   //采用中断 控制节拍    
{  TH1=0xd8;   
   TL1=0xef;   
   n--;   
}   
void FrequencyDelay (unsigned char m)   //控制频率延时    
{   
 unsigned i=3*m;   
 while(--i);   
}  
/*
	键盘扫描函数
*/
void Key_Scan()
{
	unsigned char tmp = 0xFF,i; //保存键值
	//--检测按键1--//
	if (GPIO_KEY != 0xFF)		//检测按键K1是否按下
	{
		Delayms(10);	
		tmp = GPIO_KEY;
		if (GPIO_KEY != 0xFF)	//再次检测按键是否按下
		{
			i = 0;
			while (i<40 && GPIO_KEY != 0xFF)	 //检测按键是否松开
			{
				//i++;
				//elayms(10);
			}
				
		if(tmp == 0xFF) keyValue = NOKEYPRESSED;
		else if(tmp == 0x7F) keyValue = KEY1PRESSED;
		else if(tmp == 0xBF) keyValue = KEY2PRESSED;
		else if(tmp == 0xDF) keyValue = KEY3PRESSED; 
		}
	} else {
		keyValue = NOKEYPRESSED;
	}

}

void Delayms(unsigned char a)  //豪秒延时子程序    
{   
  while(--a);                  //采用while(--a) 不要采用while(a--); 各位可编译一下看看汇编结果就知道了!    
}
/*
	时间显示状态函数
*/
void NormalWork(){
	//无闪烁模式
	flashFlag = NOFLASH;
	//更新时间
	//UpdateTime();
}
/*
	时间设置状态函数
*/
void TimeSet(){

	
	if(!setEffective)
	{
		//进入全闪烁模式
		if(flashFlag != ALLFLASH) flashFlag = ALLFLASH;
		//UpdateTime();	  	
	
	}
	if(keyValue == KEY2PRESSED)
	{
		//按下key2确认开始进行时间设置后 更新有效设置标志
		setEffective = 1;
		keyValue = NOKEYPRESSED;
		//切换闪烁模式
		flashFlag = flashFlag >= 3 ? flashFlag = HOURFLASH : flashFlag + 1;
	} else if (keyValue == KEY3PRESSED){
		keyValue = NOKEYPRESSED;
		switch(flashFlag){
			case 1: time_hour++; time_hour = time_hour % 24; break;
			case 2: time_minute++; time_minute = time_minute % 60; break;
			case 3: time_second = 0; break;	
		}
		encode();
	}

}
/*
	闹钟设置状态函数
*/
void AlarmSet(){
	if(!setEffective)
	{
		//进入全闪烁模式
		flashFlag = ALLFLASH;
	}
	if(keyValue == KEY2PRESSED)
	{
		//按下key2确认开始进行闹钟设置后 更新有效设置标志
		setEffective = 1;
		keyValue = NOKEYPRESSED;
		//切换闪烁模式
		flashFlag = flashFlag >= 3 ? flashFlag = HOURFLASH : flashFlag + 1;
	} else if (keyValue == KEY3PRESSED){
		keyValue = NOKEYPRESSED;
		switch(flashFlag){
			case 1: alarm_hour++; alarm_hour = alarm_hour % 24; break;
			case 2: alarm_minute++; alarm_minute = alarm_minute % 60; break;
			case 3: alarm_second++; alarm_second = alarm_second % 60; break;	
		}
	}
	//在闹钟设置状态下仍然要更新时间
	//UpdateTime();
}


