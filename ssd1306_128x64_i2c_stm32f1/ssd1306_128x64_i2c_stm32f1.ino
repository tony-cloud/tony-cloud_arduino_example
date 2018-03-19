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
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <EEPROM.h>
#include <Adafruit_INA219.h>
Adafruit_INA219 ina219;

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

static const unsigned char PROGMEM str_ni[] = 
{
0x08,0x80,0x08,0x80,0x08,0x80,0x11,0xFE,0x11,0x02,0x32,0x04,0x34,0x20,0x50,0x20,
0x91,0x28,0x11,0x24,0x12,0x24,0x12,0x22,0x14,0x22,0x10,0x20,0x10,0xA0,0x10,0x40
};


//在这里设置adc引脚功能
//const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
//const int analogOutPin = 9; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

//初始化rom地址
//int addr = 0;

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

void setup()   {                
  Serial.begin(115200);

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
  delay(1000);
  display.clearDisplay();
  
 // draw the first ~12 characters in the font
  testdrawchar();
  display.display();
  delay(200);
  display.clearDisplay();
  //printnihao();
  display.display();

ina219.begin(0x44);

}



void printnihao(void) {
  //drawhelloworld in Chinese   //在这里显示字符间隔+16，如(0,16,str_*,16,16,1)
  display.drawBitmap(0, 0, str_ni, 16, 16, 1); //你
  //display.setTextSize(1);
  //display.setTextColor(WHITE);
  //display.setCursor(0,32);
  //display.println(GetTemp(),1);
  //display.display();
  //delay(10);
  //display.clearDisplay();
}


void loop() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
    
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  display.print("VBUS:"); display.print(busvoltage); display.println("V");
  display.print("VS:"); display.print(shuntvoltage); display.println("mV");
  display.print("VLo:"); display.print(loadvoltage); display.println("V");
  display.print("Ic:"); display.print(current_mA); display.println("mA");
  
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.println("");


  //delay(1000);
   // read the analog in value:
  //sensorValue = analogRead(PA0);
  //printnihao();
  //display.setTextSize(1);
  //display.setTextColor(WHITE);
  //display.setCursor(0,48);
  //display.println(sensorValue);
  display.display();
  
 
}





