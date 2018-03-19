#include <DS3231.h>
#include <Wire.h>

DS3231 Clock;
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;

int year, month, date, DoW,week , hour, minute, second,temperature;

String comdata = "";
int numdata[7] = {0},  mark = 0;
/*1602液晶与UNO连接引脚
6 -> D7
7 -> D6
8 -> D5
9 -> D4
*/

int LCD1602_RS=12;   
int LCD1602_RW=11;   
int LCD1602_EN=10;   
int DB[] = { 6, 7, 8, 9};

char dis1[16]={0},dis2[16]={0};

char self_char[]={                               //1602液晶自定义符号   
        0x08,0x0f,0x12,0x0f,0x0a,0x1f,0x02,0x02, //年
        0x0f,0x09,0x0f,0x09,0x0f,0x09,0x13,0x01, //月
        0x0f,0x09,0x09,0x0f,0x09,0x09,0x0f,0x00, //日
        0x18,0x18,0x07,0x08,0x08,0x08,0x07,0x00, //温度标志— —摄氏度
        0x00,0x04,0x0E,0x1F,0x0E,0x04,0x00,0x00, //符号◆
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, //全开
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00         //
                                           };

void LCD_Command_Write(int command)             //写指令
{
int i,temp;
digitalWrite( LCD1602_RS,LOW);
digitalWrite( LCD1602_RW,LOW);
digitalWrite( LCD1602_EN,LOW);

temp=command & 0xf0;
for (i=DB[0]; i <= 9; i++)
{
   digitalWrite(i,temp & 0x80);
   temp <<= 1;
}

digitalWrite( LCD1602_EN,HIGH);
delayMicroseconds(1);
digitalWrite( LCD1602_EN,LOW);

temp=(command & 0x0f)<<4;
for (i=DB[0]; i <= 9; i++)
{
   digitalWrite(i,temp & 0x80);
   temp <<= 1;
}

digitalWrite( LCD1602_EN,HIGH);
delayMicroseconds(1); 
digitalWrite( LCD1602_EN,LOW);
}

void LCD_Data_Write(int dat)                //写数据
{
int i=0,temp;
digitalWrite( LCD1602_RS,HIGH);
digitalWrite( LCD1602_RW,LOW);
digitalWrite( LCD1602_EN,LOW);

temp=dat & 0xf0;
for (i=DB[0]; i <= 9; i++)
{
   digitalWrite(i,temp & 0x80);
   temp <<= 1;
}

digitalWrite( LCD1602_EN,HIGH);
delayMicroseconds(1);
digitalWrite( LCD1602_EN,LOW);

temp=(dat & 0x0f)<<4;
for (i=DB[0]; i <= 9; i++)
{
   digitalWrite(i,temp & 0x80);
   temp <<= 1;
}

digitalWrite( LCD1602_EN,HIGH);
delayMicroseconds(1); 
digitalWrite( LCD1602_EN,LOW);
}

void LCD_SET_XY( int x, int y )//指定坐标,x为列，0~15，y为行，0为第一行，1为第二行。
{
  int address;
  if (y ==0)    address = 0x80 + x;
  else          address = 0xC0 + x;
  LCD_Command_Write(address); 
}

void LCD_Write_Char( int x,int y,int dat)  //指定位置一个字符
{
  LCD_SET_XY( x, y ); 
  LCD_Data_Write(dat);
}

void LCD_Write_String(int X,int Y,char *s)  //指定位置字符串
{
    LCD_SET_XY( X, Y );    //设置地址 
    while (*s)             //写字符串
    {
      LCD_Data_Write(*s);   
      s ++;
    }
}

void setup (void) 
{
  Serial.begin(9600);
  int i = 0;
  for (i=6; i <= 12; i++) 
   {
     pinMode(i,OUTPUT);
   }

  delay(100);
  LCD_Command_Write(0x28);//4线 2行 5x7
  delay(50); 
  LCD_Command_Write(0x06);
  delay(50); 
  LCD_Command_Write(0x0c);
  delay(50); 
  LCD_Command_Write(0x80);
  delay(50); 
  LCD_Command_Write(0x01);
  delay(50);

    diy();              //开机问候
  //Wire.begin(); 
  Serial.println("set_time :");
  Serial.println("year mouth day week hour minute second");
  Serial.println();
  Serial.println("week : 1 -> Sunday; 2 -> Monday; 3 -> Tuesday:  ....7 -> Saturday");
  Serial.println();
  Serial.println("for example :2014-5-20 Tue 0:33:30  ");
  Serial.println("set_time :");
  Serial.println("14 5 20 3 0 33 30");
  Serial.println();
}
        

void diy()
{
     char a;
  LCD_Command_Write(0x40);   
     for(a=0;a<64;a++)
        {
          LCD_Data_Write(self_char[a]);
          delay(1);
        } 
    delay(50);
  LCD_Write_String(0,0," Digital Clock  ");
  LCD_Write_String(0,1," Geek-workshop  ");
  delay(2000);
  LCD_Command_Write(0x01);  
  delay(10);
  LCD_Write_String(0,0,"Today is new day");
  LCD_Write_String(0,1,"Dream come true!");
  delay(2000);
  LCD_Command_Write(0x01); 
delay(10);            
}

void ReadDS3231()          //读取DS3231 参数
{
  Wire.begin();

  second=Clock.getSecond();
  minute=Clock.getMinute();
  hour=Clock.getHour(h12,PM);
  week=Clock.getDoW();    
  date=Clock.getDate();
  month=Clock.getMonth(Century);
  year=Clock.getYear();
  temperature=Clock.getTemperature();
  
} 

void get_dis()          //1602液晶上每一位上显示的数据
{
  ReadDS3231();
  dis1[0]='2';
  dis1[1]='0';
  dis1[2]=0x30+year/10;
  dis1[3]=0x30+year%10;
  dis1[4]=0;
  dis1[5]=0x30+month/10;
  dis1[6]=0x30+month%10;
  dis1[7]=1;
  dis1[8]=0x30+date/10;
  dis1[9]=0x30+date%10;
  dis1[10]=2;
  dis1[11]=' ';
  dis1[12]=' ';
  switch(week)
  {
        case 2: {
                          dis1[13]='M';
                          dis1[14]='o';
                          dis1[15]='n';
                        }
                        break;
        case 3: {
                          dis1[13]='T';
                          dis1[14]='u';
                          dis1[15]='e';
                        }
                        break;
        case 4: {
                          dis1[13]='W';
                          dis1[14]='e';
                          dis1[15]='d';
                        }
                        break;
        case 5: {
                          dis1[13]='T';
                          dis1[14]='h';
                          dis1[15]='u';
                        }
                        break;
        case 6: {
                          dis1[13]='F';
                          dis1[14]='r';
                          dis1[15]='i';
                        }
                        break;
        case 7: {
                          dis1[13]='S';
                          dis1[14]='a';
                          dis1[15]='t';
                        }
                        break;
        case 1: {
                          dis1[13]='S';
                          dis1[14]='u';
                          dis1[15]='n';
                        }
                        break;
  }
  dis2[0]=' ';
  dis2[1]=0x30+hour/10;
  dis2[2]=0x30+hour%10;
  dis2[3]=':';
  dis2[4]=0x30+minute/10;
  dis2[5]=0x30+minute%10;
  dis2[6]=':';
  dis2[7]=0x30+second/10;
  dis2[8]=0x30+second%10;
  dis2[9]=' ';
  
        dis2[10]=' ';
  dis2[11]=0x30+temperature/10;
  dis2[12]=0x30+temperature%10;
  dis2[13]='.';
  dis2[14]=0x30+0;
  dis2[15]=3;
}
void play()                  //1602显示完整时间
{
       get_dis();

            int k;       
            LCD_SET_XY( 0,0);
            for(k=0;k<16;k++)
            LCD_Data_Write(dis1[k]);
            LCD_SET_XY( 0,1);
            for(k=0;k<16;k++)
            LCD_Data_Write(dis2[k]);

}

void set_time()               //DS3231设置时间
{
Wire.begin();
Clock.setSecond(numdata[6]); //秒 
Clock.setMinute(numdata[5]); //分
Clock.setHour(numdata[4]);   //时 
Clock.setDoW(numdata[3]);    //周
Clock.setDate(numdata[2]);   //日
Clock.setMonth(numdata[1]);  //月
Clock.setYear(numdata[0]);   //年
}

void loop (void)
{
     int j = 0;
  while (Serial.available() > 0)    //检测串口是否有数据
  {
    comdata += char(Serial.read());
    delay(2);
    mark = 1;
     play();
  }

  if(mark == 1)  
  {
    Serial.println(comdata);             //串口打印检测到的数据
    for(int i = 0; i < comdata.length() ; i++)
    {
      if(comdata == ' ')
      {
        j++;
      }
      else
      {
        numdata[j] = numdata[j] * 10 + (comdata - '0');
      }
    }

    comdata = String("");
   Serial.print("set_time... ");
    set_time();
    Serial.println(" OK ");
    for(int i = 0; i < 7; i++)
    {
      numdata = 0;
    }
    mark = 0;
  }
    play(); 
}
