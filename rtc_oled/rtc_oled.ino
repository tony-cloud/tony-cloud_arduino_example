double Fahrenheit(double celsius) 
{
        return 1.8 * celsius + 32;
}    //摄氏温度度转化为华氏温度

double Kelvin(double celsius)
{
        return celsius + 273.15;
}     //摄氏温度转化为开氏温度

// 露点（点在此温度时，空气饱和并产生露珠）
// 参考: http://wahiduddin.net/calc/density_algorithms.htm 
double dewPoint(double celsius, double humidity)
{
        double A0= 373.15/(273.15 + celsius);
        double SUM = -7.90298 * (A0-1);
        SUM += 5.02808 * log10(A0);
        SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
        SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
        SUM += log10(1013.246);
        double VP = pow(10, SUM-3) * humidity;
        double T = log(VP/0.61078);   // temp var
        return (241.88 * T) / (17.558-T);
}

// 快速计算露点，速度是5倍dewPoint()
// 参考: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
        double a = 17.271;
        double b = 237.7;
        double temp = (a * celsius) / (b + celsius) + log(humidity/100);
        double Td = (b * temp) / (a - temp);
        return Td;
}
static const unsigned char PROGMEM str_1[] =  
{   
0x00,0x08,0x43,0xFC,0x32,0x08,0x12,0x08,
      0x83,0xF8,0x62,0x08,0x22,0x08,0x0B,0xF8,
      0x10,0x00,0x27,0xFC,0xE4,0xA4,0x24,0xA4,
      0x24,0xA4,0x24,0xA4,0x2F,0xFE,0x20,0x00,
  };//温
  
  
static const unsigned char PROGMEM str_2[] =  
{   
0x01,0x00,0x00,0x84,0x3F,0xFE,0x22,0x20,
      0x22,0x28,0x3F,0xFC,0x22,0x20,0x23,0xE0,
      0x20,0x00,0x2F,0xF0,0x22,0x20,0x21,0x40,
      0x20,0x80,0x43,0x60,0x8C,0x1E,0x30,0x04,
  };  // 度
  
  
static const unsigned char PROGMEM str_3[] =  
{   
0x00,0x08,0x47,0xFC,0x34,0x08,0x14,0x08,
      0x87,0xF8,0x64,0x08,0x24,0x08,0x0F,0xF8,
      0x11,0x20,0x21,0x20,0xE9,0x24,0x25,0x28,
      0x23,0x30,0x21,0x24,0x3F,0xFE,0x20,0x00,
  }; //湿
#include <SPI.h>  
#include <Wire.h>  
#include <Adafruit_GFX.h>  
#include <Adafruit_SSD1306.h>  
#include <dht11.h>
#include <DS3231.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

dht11 DHT11;
#define DHT11PIN 2  
#define OLED_RESET 4  
Adafruit_SSD1306 display(OLED_RESET);  
  
#define LOGO16_GLCD_HEIGHT 16 //定义显示高度  
#define LOGO16_GLCD_WIDTH  16 //定义显示宽度  
    
#if (SSD1306_LCDHEIGHT != 64)  
#error("Height incorrect, please fix Adafruit_SSD1306.h!");  
#endif  
void printDateTime(DateTime dateTime);
//创建实例
RTC_DS1307 RTC;

DS3231 Clock;
bool Century=false;
bool h12;
bool PM;
uint8_t bell[8] = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
char data[15];
char Str_year[3], Str_month[3], Str_date[3], Str_DoW[3], Str_hour[3], Str_minute[3], Str_second[3];
String recivedStr="";//接收到的字符串
int recivedNum =0;//接收到的字符个数
void setup()   {                  
  Serial.begin(9600);  
  Wire.begin();
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)  //OLED 的I2C位置
  // init done  
    
  display.clearDisplay();  
  ////////////////
  
 // Serial.println("Please enter the time: year[2]month[2]date[2]DoW[2]hour[2]minute[2]second[2]");
 // Serial.println("example: 2014-12-3 Wednesday 14:15:15 enter:14120303141515");//1703072154
  //英文字符显示  
  //display.setTextSize(1);             //设置字体大小  
  //display.setTextColor(WHITE);        //设置字体颜色白色  
  //display.setCursor(0,0);             //设置字体的起始位置  
  //display.println("   zhongbest.com");//输出字符并换行   
  //display.display();                  //把缓存的都显示  
  //初始化实时时钟
  RTC.begin();
}  
  
void loop() {  
   int chk = DHT11.read(DHT11PIN);

 // Serial.print("Read sensor: ");
//  switch (chk)
//  {
//    case DHTLIB_OK: 
//                Serial.println("OK"); 
//                break;
//    case DHTLIB_ERROR_CHECKSUM: 
//                Serial.println("Checksum error"); 
//                break;
//    case DHTLIB_ERROR_TIMEOUT: 
//                Serial.println("Time out error"); 
//                break;
//    default: 
//                Serial.println("Unknown error"); 
//                break;
//  }
  display.clearDisplay();
  //英文字符显示  
  display.setTextSize(1);             //设置字体大小  
  display.setTextColor(WHITE);        //设置字体颜色白色  
  display.setCursor(0,0);             //设置字体的起始位置  
   
 // display.print("Humidity(%):");
 // display.println((float)DHT11.humidity, 2);

 // display.print("Temperature(oC):");
  

//  display.print("Temperature(oF):");
//  display.println(Fahrenheit(DHT11.temperature), 2);
//
//  display.print("Temperature(K):");
//  display.println(Kelvin(DHT11.temperature), 2);
//
//  display.print("Dew Point(oC):");
//  display.println(dewPoint(DHT11.temperature, DHT11.humidity));
//
//  display.print("Dew PointFast(oC):");
//  display.println(dewPointFast(DHT11.temperature, DHT11.humidity));
  //中文字符显示  
  display.drawBitmap(0, 5, str_1, 16, 16, 1); //在坐标X:26  Y:16的位置显示中文字符中
  display.drawBitmap(18, 5, str_2, 16, 16, 1); //在坐标X:42  Y:16的位置显示中文字符贝
  display.setCursor(36,11);             //设置字体的起始位置
  display.setTextSize(1);             //设置字体大小
  display.print(":"); 
  display.println((float)DHT11.temperature, 2);
   display.drawBitmap(0, 25, str_3, 16, 16, 1); //在坐标X:26  Y:16的位置显示中文字符中
  display.drawBitmap(18, 25, str_2, 16, 16, 1); //在坐标X:42  Y:16的位置显示中文字符贝
  display.setCursor(36,30);             //设置字体的起始位置
   display.setTextSize(1);             //设置字体大小
  display.print(":"); 
  display.println((float)DHT11.humidity, 2);
  int val;
  val=analogRead(0);
 // Serial.println(val,DEC);
  display.setCursor(0,55);             //设置字体的起始位置
  display.print("MQ-2: ");
  display.println(val,DEC);
 // display.setCursor(0,60);
  display.print("-------------------"); 
  DateTime now = RTC.now();
//通过串口传送当前的日期和时间      
  printDateTime(now);

  ///////////////////////////////////////////////////////////

   while(Serial.available() > 0)//读取字符串
   {
      recivedStr += char(Serial.read());
//      delay(2);
      recivedNum = recivedNum+1;
   }
   if(recivedNum == 12)
   {  //year[2]month[2]date[2]DoW[2]hour[2]minute[2]second[2]
       String strtemp = "";
       strtemp = recivedStr.substring(0,2);
       int years = strtemp.toInt();
       RTC.set(RTC_YEAR,years);//设置年
     
       strtemp = recivedStr.substring(2,4);
       int months = strtemp.toInt();
       RTC.set(RTC_MONTH,months);//设置月
       
       strtemp = recivedStr.substring(4,6);
       int days = strtemp.toInt();
       RTC.set(RTC_DAY,days);//设置日
       
       strtemp = recivedStr.substring(6,8);
       int hours = strtemp.toInt();
       RTC.set(RTC_HOUR,hours);//设置小时

       strtemp = recivedStr.substring(8,10);
       int minutes = strtemp.toInt();
       RTC.set(RTC_MINUTE,minutes);//设置分钟
        
       strtemp = recivedStr.substring(10,12);
       int seconds = strtemp.toInt();
       RTC.set(RTC_SECOND,seconds);//设置秒
       recivedNum = 0;
   }
  display.display();                  //把缓存的都显示 
  delay(1000);
} 

void printDateTime(DateTime dateTime) {
    //传送年份
     display.setCursor(0,45);             //设置字体的起始位置
    display.print(dateTime.year(), DEC);
    display.print('/');
    //传送月份
    display.print(dateTime.month(), DEC);
    display.print('/');
    //传送月份中的第几天
    display.print(dateTime.day(), DEC);
    display.print(' ');
    //传送小时
    display.print(dateTime.hour(), DEC);
    display.print(':');
    //传送分钟
    display.print(dateTime.minute(), DEC);
    display.print(':');
    //传送秒
    display.print(dateTime.second(), DEC);
    display.display();     
