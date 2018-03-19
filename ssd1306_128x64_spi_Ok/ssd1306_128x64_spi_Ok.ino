/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using SPI to communicate
4 or 5 pins are required to interface

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

// If using software SPI (the default case):
#define OLED_MOSI   PA9
#define OLED_CLK   PA10
#define OLED_DC    PA11
#define OLED_CS    PA12
#define OLED_RESET PA13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/* Uncomment this block to use hardware SPI
#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
*/

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
int mode=1;


#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {                
  Serial.begin(9600);
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done


  delay(1000);
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

 
}


void loop() {
   display.display();
  delay(1000);
 display.clearDisplay();
if (mode == 1){
mode=0;
display.invertDisplay(false);
  display.clearDisplay();
    display.display();
        // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
   delay(1000);
  display.println("Hello, world!");
    display.display();
   delay(3000);
  display.println("This is a test text.");
   display.display();
   delay(2000);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Hey, there!.");
   display.display();
   delay(2000);
  display.println("I'm a test program.");
   display.display();
   delay(2000);
  display.println("I'm glad to meet you.");
   display.display();
   delay(2000); 
  display.println("Thanks!.");
   display.display();
   delay(2000); 
  display.println("Bye~");
   display.display();
   delay(2000); 
  display.display();
  delay(2000);
delay(1000); 
}

if (mode == 0){
mode=1;
display.invertDisplay(true);
  display.clearDisplay();
    display.display();
        // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
   delay(1000);
  display.println("Hello, world!");
    display.display();
   delay(3000);
  display.println("This is a test text.");
   display.display();
   delay(2000);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Hey, there!.");
   display.display();
   delay(2000);
  display.println("I'm a test program.");
   display.display();
   delay(2000);
  display.println("I'm glad to meet you.");
   display.display();
   delay(2000); 
  display.println("Thanks!.");
   display.display();
   delay(2000); 
  display.println("Bye~");
   display.display();
   delay(2000); 
  display.display();
  delay(2000);
delay(1000); 
}


}





