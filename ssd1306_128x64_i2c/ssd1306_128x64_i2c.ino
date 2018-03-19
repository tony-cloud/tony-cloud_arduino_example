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

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

//中文字库在这里
//你
static const unsigned char PROGMEM str_ni[] = 
{
0x08,0x80,0x08,0x80,0x08,0x80,0x11,0xFE,0x11,0x02,0x32,0x04,0x34,0x20,0x50,0x20,
0x91,0x28,0x11,0x24,0x12,0x24,0x12,0x22,0x14,0x22,0x10,0x20,0x10,0xA0,0x10,0x40
};
//好
static const unsigned char PROGMEM str_hao[] = 
{
0x10,0x00,0x10,0xFC,0x10,0x04,0x10,0x08,0xFC,0x10,0x24,0x20,0x24,0x20,0x25,0xFE,
0x24,0x20,0x48,0x20,0x28,0x20,0x10,0x20,0x28,0x20,0x44,0x20,0x84,0xA0,0x00,0x40
};
//，
static const unsigned char PROGMEM str_dou[] = 
{
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x30,0x00,0x10,0x00,0x20,0x00,0x00,0x00
};
//世
static const unsigned char PROGMEM str_shi[] = 
{
0x02,0x20,0x12,0x20,0x12,0x20,0x12,0x20,0x12,0x20,0xFF,0xFE,0x12,0x20,0x12,0x20,
0x12,0x20,0x12,0x20,0x13,0xE0,0x10,0x00,0x10,0x00,0x10,0x00,0x1F,0xFC,0x00,0x00
};
//界
static const unsigned char PROGMEM str_jie[] = 
{
0x00,0x00,0x1F,0xF0,0x11,0x10,0x11,0x10,0x1F,0xF0,0x11,0x10,0x11,0x10,0x1F,0xF0,
0x02,0x80,0x0C,0x60,0x34,0x58,0xC4,0x46,0x04,0x40,0x08,0x40,0x08,0x40,0x10,0x40
};

void setup()   {                
  Serial.begin(9600);

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
  delay(2000);
  display.clearDisplay();
  
 // draw the first ~12 characters in the font
  testdrawchar();
  display.display();
  delay(2000);
  display.clearDisplay();

  

}


void loop() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Hello, world!");
  display.display();
  delay(2000);
  display.clearDisplay();

  //drawhelloworld in Chinese   //在这里显示字符间隔+16，如(0,16,str_*,16,16,1)
  display.drawBitmap(0, 0, str_ni, 16, 16, 1); //你
  display.drawBitmap(16, 0, str_hao, 16, 16, 1);
  display.drawBitmap(32, 0, str_dou, 16, 16, 1);
  display.drawBitmap(48, 0, str_shi, 16, 16, 1);
  display.drawBitmap(64, 0, str_jie, 16, 16, 1);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,32);
  display.println("From Tony");
  display.display();
  delay(2000);
  display.clearDisplay();

  //draw everystring
  testdrawchar();
  display.display();
  delay(2000);
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




