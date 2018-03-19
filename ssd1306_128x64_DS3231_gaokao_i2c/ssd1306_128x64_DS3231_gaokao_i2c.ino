/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

//#include <SPI.h>
#include <WireSoft.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <EEPROM.h>
#include <DS3231.h>

DS3231 Clock;
bool Century=false;
bool h12;
bool PM;
byte Date, Hour, Minute, Second, Month, Year, Week;
bool ADy, A12h, Apm;

CTwoWireSoft WireSoft(PA1, PA2, SOFT_STANDARD);
TwoWire myWire(PA1,PA2);

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2



#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 


#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

char byte1;  
String str1 = "";

//中文字库在这里
//距
static const unsigned char PROGMEM str_ju[] = 
{
0x00,0x00,0x7D,0xFE,0x45,0x00,0x45,0x00,0x45,0x00,0x7D,0xFC,0x11,0x04,0x11,0x04,
0x5D,0x04,0x51,0x04,0x51,0xFC,0x51,0x00,0x5D,0x00,0xE1,0x00,0x01,0xFE,0x00,0x00
};
//高
static const unsigned char PROGMEM str_gao[] = 
{
0x02,0x00,0x01,0x00,0xFF,0xFE,0x00,0x00,0x0F,0xE0,0x08,0x20,0x08,0x20,0x0F,0xE0,
0x00,0x00,0x7F,0xFC,0x40,0x04,0x4F,0xE4,0x48,0x24,0x48,0x24,0x4F,0xE4,0x40,0x0C
};
//考
static const unsigned char PROGMEM str_kao[] = 
{
0x02,0x00,0x02,0x08,0x3F,0xD0,0x02,0x20,0x02,0x40,0xFF,0xFE,0x01,0x00,0x02,0x00,
0x0F,0xF8,0x12,0x00,0x24,0x00,0x47,0xF0,0x80,0x10,0x00,0x10,0x00,0xA0,0x00,0x40
};
//天
static const unsigned char PROGMEM str_tian[] = 
{
0x00,0x00,0x3F,0xF8,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0xFF,0xFE,0x01,0x00,
0x02,0x80,0x02,0x80,0x04,0x40,0x04,0x40,0x08,0x20,0x10,0x10,0x20,0x08,0xC0,0x06
};


//在这里设置adc引脚功能
//const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
//const int analogOutPin = 9; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

//初始化rom地址
int addr = 0;

//倒计时模块int days=GetDays(2018,06,07);
//获取2个日期之间相差的天数
int GetDays(int iYear1, int iMonth1, int iDay1)   //1. 确保 日期1 < 日期2
{
  int iYear2 = Clock.getYear()+2000;
  int iMonth2 = Clock.getMonth(Century);
  int iDay2 = Clock.getDate();
  unsigned int iDate1 = iYear1 * 10000 + iMonth1 * 100 + iDay1;
  unsigned int iDate2 = iYear2 * 10000 + iMonth2 * 100 + iDay2;
  if (iDate1 > iDate2)
  {
    iYear1 = iYear1 + iYear2 - (iYear2 = iYear1);
    iMonth1 = iMonth1 + iMonth2 - (iMonth2 = iMonth1);
    iDay1 = iDay1 + iDay2 - (iDay2 = iDay1);
  }
   
  //2. 开始计算天数
  //计算 从 iYear1年1月1日 到 iYear2年1月1日前一天 之间的天数
  int iDays = 0;
  for (int i = iYear1; i < iYear2; i++)
  {
    iDays += (IsLeapYear(i) ? 366 : 365);
  }
   
  //减去iYear1年前iMonth1月的天数
  for (int i = 1; i < iMonth1; i++)
  {
    switch (i)
    {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
      iDays -= 31;
      break;
    case 4: case 6: case 9: case 11:
      iDays -= 30;
      break;
    case 2:
      iDays -= (IsLeapYear(iYear1) ? 29 : 28);
      break;
    }
  }
  //减去iYear1年iMonth1月前iDay1的天数
  iDays -= (iDay1 - 1);
 //加上iYear2年前iMonth2月的天数
 for (int i = 1; i < iMonth2; i++)
  {
    switch (i)
    {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
      iDays += 31;
      break;
    case 4: case 6: case 9: case 11:
      iDays += 30;
      break;
    case 2:
      iDays += (IsLeapYear(iYear2) ? 29 : 28);
      break;
    }
  }
  //加上iYear2年iMonth2月前iDay2的天数
  iDays += (iDay2 - 1);
  if (iDays <= 0)
{
  iDays = 0;
  }
  
  return iDays;
}
 
  //判断给定年是否闰年
bool IsLeapYear(int iYear)
{
  if (iYear % 100 == 0)
    return ((iYear % 400 == 0));
  else
    return ((iYear % 4 == 0));
}

void setup()   {                
  //Wire.stm32SetSDA(PA10);
  //Wire.stm32SetSCL(PA9);
  //TwoWire myWire(SDA,SCL);
  //Wire.begin();
  TwoWire myWire(PA6,PA5);

  Wire.begin();
  // Start the serial interface
  SerialUART1.stm32SetRX(PA3);
  SerialUART1.stm32SetTX(PA2);
  SerialUART1.begin(115200);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  

  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  // draw a single pixel
  display.drawPixel(10, 10, WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  display.display();
  delay(500);
  display.clearDisplay();
  
 // draw the first ~12 characters in the font
  testdrawchar();
  display.display();
  delay(500);
  
  display.print("Initial....");
  display.display();
  delay(1000);
  display.clearDisplay();
}

void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }    
  display.display();
  delay(1);
}

void printgaokao(void) {
  //drawhelloworld in Chinese   //在这里显示字符间隔+16，如(0,16,str_*,16,16,1)
  display.drawBitmap(0, 48, str_ju, 16, 16, 1); //距
  display.drawBitmap(16, 48, str_gao, 16, 16, 1);//高
  display.drawBitmap(32, 48, str_kao, 16, 16, 1);//考
  display.setCursor(48,48);
  display.setTextSize(2);
  int days=GetDays(2018,06,07);
  display.print(days, DEC);
  display.drawBitmap(90, 48, str_tian, 16, 16, 1);//天
  //display.drawBitmap(64, 0, str_jie, 16, 16, 1);
  //display.setTextSize(1);
  //display.setTextColor(WHITE);
  //display.setCursor(0,32);
  //display.println(GetTemp(),1);
  //display.display();
  //delay(10);
  //display.clearDisplay();
}


void loop() {
   // read the analog in value:
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();

// send what's going on to the serial monitor.
  // Start with the year
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("2");
  if (Century) {      // Won't need this for 89 years.
    display.print("1");
  } else {
    display.print("0");
  }
  Date = Clock.getDate();
  Hour = Clock.getHour(h12, PM);
  Minute = Clock.getMinute();
  Second = Clock.getSecond();
  Month = Clock.getMonth(Century);
  Week = Clock.getDoW();

  display.print(Clock.getYear(), DEC);
  display.print('-');
  // then the month
  if (Month <10)
  {
    display.print("0");
  }
  display.print(Month, DEC);
  display.print('-');
  // then the date
  if (Date <10)
  {
    display.print("0");
  }
  display.print(Date, DEC);
  //display.print('');
 
  // and the day of the week
  display.setCursor(0,16);
  if (Week == 1)
  {
    display.print("Sunday");
  }
  else if (Week == 2)
  {
    display.print("Monday");
  }
  else if (Week == 3)
  {
    display.print("Tuesday");
  }
  else if (Week == 4)
  {
    display.print("Wednesday");
  }
  else if (Week == 5)
  {
    display.print("Thursday");
  }
  else if (Week == 6)
  {
    display.print("Friday");
  }
  else if (Week == 7)
  {
    display.print("Saturday");
  }
  //display.print(Clock.getDoW(), DEC);
  //display.print(' ');

  
  display.setCursor(0,32);
  // Finally the hour, minute, and second
  if (Hour <10)
  {
    display.print("0");
  }
  display.print(Hour, DEC);
  display.print(':');
  if (Minute <10)
  {
    display.print("0");
  }
  display.print(Minute, DEC);
  display.print(':');
  if (Second <10)
  {
    display.print("0");
  }
  display.print(Second, DEC);

  


//高考倒计时
printgaokao();
//display.setCursor(0,48);
//display.setTextSize(1);
//int days=GetDays(2018,06,07);
//display.print(days, DEC);
//display.print("days before CEE");
  // Display the temperature
  //display.print("T=");
  //display.print(Clock.getTemperature(), 2);
  //display.print(" C");
  
  // Indicate whether an alarm went off

  //display.print('\n');
  display.display();
  delay(1);
  
 
}





