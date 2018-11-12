#include<reg51.h>

#define GPIO_DIG   P0	//����ܶ�ѡ
#define GPIO_PLACE P1	//�����λѡ
#define GPIO_KEY P2  //����������P2��
/*
	keyValue
*/
#define NOKEYPRESSED -1 //�ް�������
#define KEY1PRESSED 1 //����1������
#define KEY2PRESSED 2 //����2������
#define KEY3PRESSED 3 //����3������
/*
	flashLightFlag
*/
#define DARK -1 //��
#define LIGHT 1 //��
/*
	flashLocationFlag
*/
#define NOFLASH 0 //����˸
#define HOURFLASH 1 //ʱ������˸
#define MINUTEFLASH 2 //��������˸
#define SECONDFLASH 3 //��������˸
#define ALLFLASH 4 //����������˸
/*
	stateFlag
*/
#define NORMALSTATE 0 //ʱ����ʾģʽ
#define TIMESETSTATE 1 //ʱ������ģʽ
#define ALARMSETSTATE 2 //��������ģʽ
#define ALARMSTATE 3 //����ģʽ

//--����ȫ�ֱ���--//

//λѡ����   ���ķ�������
unsigned char code DIG_PLACE[8] = {
0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
//0��1��2��3��4��5��6��7��8��9��A��b��C��d��E��F����ʾ��
unsigned char code DIG_CODE[17] = {
0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

sbit Beep =  P3^0 ; 
   
unsigned char code music_tab[] ={   
0x18, 0x30, 0x1C , 0x10, //��ʽΪ: Ƶ�ʳ���, ���ĳ���, Ƶ�ʳ���, ���ĳ���,    
0x20, 0x40, 0x1C , 0x10,   
0x18, 0x10, 0x20 , 0x10,   
0x1C, 0x10, 0x18 , 0x40,   
0x1C, 0x20, 0x20 , 0x20,   
0x1C, 0x20, 0x18 , 0x20,   
0x20, 0x80, 0x20 , 0x30, 
0x1C, 0x10, 0x18 , 0x20,
0x15, 0x20, 0x1C , 0x20, 
0x20, 0x20, 0x26 , 0x40,
0x20, 0x20, 0x2B , 0x20,
0x26, 0x20, 0x20 , 0x20,
0x30, 0x80, 0x20 , 0x20, 0x1C , 0x10,   
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

//�������Ҫ��ʾ��8λ����ֵ
unsigned char DisplayData[8];
//��ǰʱ��
unsigned char time_hour,time_minute,time_second;
//����ʱ��
unsigned char alarm_hour,alarm_minute,alarm_second;
//��ǰ������ֵ
int keyValue = NOKEYPRESSED;
//��˸������־
int flashLightFlag = DARK;
//��˸λ�ñ�־
unsigned char flashLocationFlag = NOFLASH;
//����״̬��־
unsigned char stateFlag = NORMALSTATE;
//��Ч���ñ�־ 
//����û�������״̬�н�������Ч���� ����״̬ת��ʱ �Իص�ԭ״̬���û��鿴���ý��
unsigned char setEffective = 0; 

//--����ȫ�ֺ���--//
void DigDisplay(); //�������ʾ����
void delay(); //��ʱ����
void Init(); //��ʼ������
void encode(); //���뺯��
void UpdateTime(); //ʱ����º���
void UpdateState(); //����״̬���º���
void Key_Scan(); //������⺯��
void Delayms(unsigned char a);
void NormalWork(); //ʱ����ʾ״̬����
void TimeSet();	//ʱ������״̬����
void AlarmSet(); //��������״̬����
void Alarm(); //����״̬����
void Sing(); //�������庯��
int AlarmTime(); //��������ʱ��
void FrequencyDelay (unsigned char m); //������ӦƵ�ʵ�PWM������ʱ

void main(void)
{
	Init();
	encode();
	while(1)
	{
		/*
			����״̬��
		*/
		switch(stateFlag){
	    	case NORMALSTATE:
			  	//ʱ����ʾģʽ
				NormalWork();
				break;
			case TIMESETSTATE:
				//ʱ������ģʽ
				TimeSet();	
				break;
			case ALARMSETSTATE:
				//��������ģʽ
				AlarmSet();
				break;
			case ALARMSTATE:
				//����ģʽ
				Alarm();
				break;
					
		}
		//���¹���״̬
		UpdateState();
	

	}				
}

/*
	��ʼ��
*/
void Init(){

	//��ʼ����ʱ������
	TH0 = (65536 - 50000)/256;
	TL0 = (65536 - 50000)%256;
	TMOD = 0x11;
	EA = 1;
	ET0 = 1;
	ET1 = 1;
	TR0 = 1;
	//��ʼ��ʱ��
	time_hour = 23;
	time_minute = 59;
	time_second = 50;
	//��ʼ������
	alarm_hour = 00;
	alarm_minute = 00;
	alarm_second = 00;
}
/*
	ʱ����º��� ����ǰʱ��ȫ�ֱ�������1��
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
	����״̬���º��� ����key1״̬ ���¹���״̬ȫ�ֱ���
*/
void UpdateState(){

	//�����������ʱ���ת��������״̬
	if(stateFlag != ALARMSTATE)
	{
		if(time_hour == alarm_hour & time_minute == alarm_minute & time_second == alarm_second)
		{
			stateFlag = ALARMSTATE;
			return;	
		}
	//���������״̬�� �Ͳ���Ӧ����״̬ת�� ����������״̬�����н��д���
	}else return;
	
	//���key1������ ���¹���״̬
	if(keyValue == KEY1PRESSED){
		//���Ѱ���
		keyValue = NOKEYPRESSED;
		//���¹���״̬��־
		if(setEffective){
			//�����������Ч���� ����Ч���ñ�־���� ���ص�ԭ״̬���û��鿴���ý��
			setEffective = 0;
		 	stateFlag = stateFlag;
		} 
		else
			stateFlag = stateFlag == 2 ? NORMALSTATE : stateFlag+1; 
	}
	

}
/*
	���뺯�� ��int�����ֽ���Ϊ���ݸ��������ʾ�Ķ��� װ��DisplayData
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
	
	//Сʱʮλ
	tens_place = hour / 10; 
	//Сʱ��λ
	ones_place = (hour - (tens_place * 10));
	DisplayData[0] = DIG_CODE[tens_place];	  
	DisplayData[1] = DIG_CODE[ones_place];
	//����ʮλ
	tens_place = minute / 10; 
	//���Ӹ�λ
	ones_place = (minute - (tens_place * 10));
	DisplayData[2] = DIG_CODE[tens_place];	  
	DisplayData[3] = DIG_CODE[ones_place];
	//����ʮλ
	tens_place = second / 10; 
	//���Ӹ�λ
	ones_place = (second - (tens_place * 10));
	DisplayData[4] = DIG_CODE[tens_place];	  
	DisplayData[5] = DIG_CODE[ones_place];
	
	DisplayData[6] = 0x00;
	DisplayData[7] = 0x00;
}
/*
	�������ʾDisplayData
*/

void DigDisplay()
{
	unsigned char i;
	unsigned int j;
	for(i=0; i<8; i++)
	{
		if(flashLightFlag == DARK & flashLocationFlag != NOFLASH)
		{
			if(flashLocationFlag == ALLFLASH)
			{
				GPIO_PLACE = DIG_PLACE[i];
				GPIO_DIG = 0x00;
			}
			else if(i == (flashLocationFlag)*2-2 || i == (flashLocationFlag)*2-1)
			{
				GPIO_PLACE = DIG_PLACE[i];
				GPIO_DIG = 0x00;
			} else {
				GPIO_PLACE = DIG_PLACE[i];	 //����λѡ
				GPIO_DIG = DisplayData[i];     //���Ͷ���			
			}
		} else {
			GPIO_PLACE = DIG_PLACE[i];	 //����λѡ
			GPIO_DIG = DisplayData[i];     //���Ͷ���
			
		}
		j = 100;
		while(--j);
		GPIO_DIG = 0x00;//����
	}
	
}

/*	
	��ʱ
*/
unsigned char num = 0; //��ʱ����ʱ��־����
unsigned char oneSecondEndFlag = 0;
unsigned char alarmFlag = 0;
void delay(){	
	
	if(stateFlag == ALARMSTATE)
	{
		Sing();
	}
	//��ʾ�������
	DigDisplay();
	if (GPIO_KEY != 0xFF)
	{
		Key_Scan();
	}

}
//��ʱ��0���жϷ������
void T0InterruptService() interrupt 1{
	TH0 = (65536 - 50000)/256;
	TL0 = (65536 - 50000)%256;
	num++;
	//Alarm2�������־
	/*
	if(num%2 == 0){
		alarmFlag = (alarmFlag == 0? 1 : 0);
	}
	*/
	if(num == 10) 
	{
		flashLightFlag = LIGHT;
	}
	if(num == 20)
	{
		num = 0;
		oneSecondEndFlag = 1;
		flashLightFlag = DARK;
		if(stateFlag != TIMESETSTATE | setEffective != 1)
		{
			UpdateTime();
			encode();
		}
			
		
	}

}
/*
	��������
*/
unsigned char frequency,beat,musicIndex=0,alarmEnd = 0;   //mΪƵ�ʳ������� 
void Sing(){	 
	frequency = music_tab[musicIndex];   
	if(frequency==0x00) 
	{
		alarmEnd = 1; 
	} else {
		//ȡƵ�ʳ��� �� ���ĳ���
		beat = music_tab[++musicIndex];
		musicIndex++;
	}     
	//��ʱ��1�������ļ�ʱ
	TR1=1;    
	while(beat != 0) 
	{
		//���ɶ�ӦƵ�ʵ�PWM��
		Beep=~Beep;
		FrequencyDelay(frequency);
	} 
	TR1=0;



}
//�����򵥵�����
void Alarm2(){
	Beep=~Beep,DigDisplay(),FrequencyDelay(1);
}
void T1InterruptService() interrupt 3    
{  TH1=0xd8;   
   TL1=0xef;   
   beat--;   
}   
void FrequencyDelay (unsigned char m)   //����Ƶ����ʱ    
{   
 unsigned i = 3*m;   
 while(--i) ;   
}  
/*
	����ɨ�躯��
*/
void Key_Scan()
{
	unsigned char tmp = 0xFF,i; //�����ֵ
	//--��ⰴ��1--//
	if (GPIO_KEY != 0xFF)		//��ⰴ��K1�Ƿ���
	{
		Delayms(10);	
		tmp = GPIO_KEY;
		if (GPIO_KEY != 0xFF)	//�ٴμ�ⰴ���Ƿ���
		{
			i = 0;
			while (i<40 && GPIO_KEY != 0xFF)	 //��ⰴ���Ƿ��ɿ�
			{
				
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

void Delayms(unsigned char a)  //������ʱ�ӳ���    
{   
  while(--a);                  //����while(--a) ��Ҫ����while(a--); ��λ�ɱ���һ�¿����������֪����!    
}
/*
	ʱ����ʾ״̬����
*/
void NormalWork(){
	//����˸ģʽ
	flashLocationFlag = NOFLASH;
	//��ʾ�������
	DigDisplay();
	if (GPIO_KEY != 0xFF)
	{
		Key_Scan();
	}
}
/*
	ʱ������״̬����
*/
void TimeSet(){

	
	if(!setEffective)
	{
		//����ȫ��˸ģʽ
		if(flashLocationFlag != ALLFLASH) flashLocationFlag = ALLFLASH;
		//UpdateTime();	  	
	
	}
	if(keyValue == KEY2PRESSED)
	{
		keyValue = NOKEYPRESSED;
		//����key2ȷ�Ͽ�ʼ����ʱ�����ú� ������Ч���ñ�־
		setEffective = 1;
		//�л���˸ģʽ
		flashLocationFlag = flashLocationFlag >= SECONDFLASH ? flashLocationFlag = HOURFLASH : flashLocationFlag + 1;
	} else if (keyValue == KEY3PRESSED){
		keyValue = NOKEYPRESSED;
		//������˸λ���޸Ķ�Ӧ��ֵ
		switch(flashLocationFlag){
			case 1: time_hour++; time_hour = time_hour % 24; break;
			case 2: time_minute++; time_minute = time_minute % 60; break;
			case 3: time_second = 0; break;	
		}
		encode();
	}
	//��ʾ�������
	//DigDisplay();
	if (GPIO_KEY != 0xFF)
	{
		Key_Scan();
	}

}
/*
	��������״̬����
*/
void AlarmSet(){
	if(!setEffective)
	{
		//����ȫ��˸ģʽ
		if(flashLocationFlag != ALLFLASH) flashLocationFlag = ALLFLASH;
	}
	if(keyValue == KEY2PRESSED)
	{
		//����key2ȷ�Ͽ�ʼ�����������ú� ������Ч���ñ�־
		setEffective = 1;
		keyValue = NOKEYPRESSED;
		//�л���˸ģʽ
		flashLocationFlag = flashLocationFlag >= 3 ? flashLocationFlag = HOURFLASH : flashLocationFlag + 1;
	} else if (keyValue == KEY3PRESSED){
		keyValue = NOKEYPRESSED;
		//������˸λ���޸Ķ�Ӧ��ֵ
		switch(flashLocationFlag){
			case 1: alarm_hour++; alarm_hour = alarm_hour % 24; break;
			case 2: alarm_minute++; alarm_minute = alarm_minute % 60; break;
			case 3: alarm_second++; alarm_second = alarm_second % 60; break;	
		}
		encode();
	}
	//��ʾ�������
	DigDisplay();
	if (GPIO_KEY != 0xFF)
	{
		Key_Scan();
	}

}

/*
	����״̬����
*/
void Alarm(){
	
	unsigned char sum;
	if(!setEffective)
	{
		//�رն�ʱ��0 �������жϷ����������������Ƶ�Ӱ��
		ET0 = 0;
		//��ʱ��־��0
		num = 0;
		setEffective = 1;
	} else {
		Sing();
		if(alarmEnd || keyValue == KEY1PRESSED)
		{
			//���Ѱ���
			keyValue = NOKEYPRESSED;
			//����ʱ��0
			ET0 = 1;
			//�л���ʱ����ʾģʽ
			stateFlag = NORMALSTATE;	
			//���Ѿ����������ʱ�����
			sum = AlarmTime();
			time_second += sum;
			if(time_second > 60)
			{
				time_minute ++;
				time_second -= 60;
			} 
			//������������
			musicIndex = 0;

		}
	
	}
	if (GPIO_KEY != 0xFF)
	{
		Key_Scan();
	}


}
int AlarmTime(){
	unsigned char i;
	int sum = 0;
	for(i = 0;i<musicIndex/2;i++){
		sum += music_tab[2*i+1];
	}
	return sum/100;	
}