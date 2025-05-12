// ESP8266_ST7735_Adfruit_demo_nn
// microcontroller: ESP8266
// display: 8-pin 128*160 SPI TFT (SD card reader not used) controller ST7735S
// Example in the Adafruit ST7735 and ST7789 library
//
// here used with the ESP8266 microcontroller
// pin assignment
//
// ESP8266   ======   128*160 SPI TFT
//   3V    --------------- VCC
//   GND   --------------- GND
//   D8    --------------- CS
//   D3    --------------- RESET
//   D4    --------------- A0
//   D7    --------------- SDA
//   D5    --------------- SCK
//   3V    --------------- LED
//
// note that elements redundant to the current display have been deleted from the example
// public domain
// Floris Wouterlood
// January 1, 2023


/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
    ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
    ----> https://www.adafruit.com/product/2088
  The 1.14" TFT breakout
  ----> https://www.adafruit.com/product/4383
  The 1.3" TFT breakout
  ----> https://www.adafruit.com/product/4313
  The 1.54" TFT breakout
    ----> https://www.adafruit.com/product/3787
  The 1.69" TFT breakout
    ----> https://www.adafruit.com/product/5206
  The 2.0" TFT breakout
    ----> https://www.adafruit.com/product/4311
  as well as Adafruit raw 1.8" TFT display
    ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/


#include <Adafruit_GFX.h>    // core graphics library
#include <Adafruit_ST7735.h> // hardware-specific library for ST7735
#include <SPI.h>

#define TFT_CS         D8
#define TFT_RST        D3
#define TFT_DC         D4
#define shortdelay    500

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

float p = 3.1415926;


void setup(void) {
  Serial.begin (9600);
  Serial.println ();
  Serial.println ();   
  delay (shortdelay*2);   
  Serial.println (F("Hello! ST7735 180*160 SPI TFT Test"));

  tft.initR (INITR_BLACKTAB);      // init ST7735S chip, black tab
  Serial.println (F("Initialized"));
}


void loop() {
  uint16_t time = millis();
  tft.fillScreen (ST77XX_BLACK);
  time = millis() - time;

  Serial.println (time, DEC);
  delay (shortdelay);

  // large block of text
  tft.fillScreen (ST77XX_BLACK);
  testdrawtext ("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere bladebla.", ST77XX_WHITE);
  delay (shortdelay*2);

  // tft print function!
  tftPrintTest();
  delay (shortdelay*8);

  // a single pixel
  tft.drawPixel (tft.width()/2, tft.height()/2, ST77XX_GREEN);
  delay (shortdelay);

  // line draw test
  testlines (ST77XX_YELLOW);
  delay (shortdelay);

  // optimized lines
  testfastlines (ST77XX_RED, ST77XX_BLUE);
  delay (shortdelay);

  testdrawrects (ST77XX_GREEN);
  delay (shortdelay);

  testfillrects (ST77XX_YELLOW, ST77XX_MAGENTA);
  delay (shortdelay);

  tft.fillScreen (ST77XX_BLACK);
  testfillcircles (10, ST77XX_BLUE);
  testdrawcircles (10, ST77XX_WHITE);
  delay (shortdelay);

  testroundrects();
  delay (shortdelay);

  testtriangles();
  delay (shortdelay);

  mediabuttons();
  delay (shortdelay);

  Serial.println ("done");
  delay (shortdelay*2);

  tft.invertDisplay (true);
  delay (shortdelay);
  tft.invertDisplay (false);
  delay (shortdelay);
  tft.invertDisplay (true);
  delay (shortdelay);
  tft.invertDisplay (false);
  delay (shortdelay);
  tft.invertDisplay (true);
  delay (shortdelay);
  tft.invertDisplay (false);
  delay (shortdelay);
}


void testlines (uint16_t color) {
  tft.fillScreen (ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine (0, 0, x, tft.height()-1, color);
    delay (0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine (0, 0, tft.width()-1, y, color);
    delay (0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine (tft.width()-1, 0, x, tft.height()-1, color);
    delay (0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine (tft.width()-1, 0, 0, y, color);
    delay (0);
  }

  tft.fillScreen (ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine (0, tft.height()-1, x, 0, color);
    delay (0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine (0, tft.height()-1, tft.width()-1, y, color);
    delay (0);
  }

  tft.fillScreen (ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine (tft.width()-1, tft.height()-1, x, 0, color);
    delay (0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine (tft.width()-1, tft.height()-1, 0, y, color);
    delay (0);
  }
}


void testdrawtext (char *text, uint16_t color) {
  tft.setCursor (0, 0);
  tft.setTextColor (color);
  tft.setTextWrap (true);
  tft.print (text);
}


void testfastlines (uint16_t color1, uint16_t color2) {
  tft.fillScreen (ST77XX_BLACK);
  for (int16_t y=0; y < tft.height (); y+=5) {
    tft.drawFastHLine (0, y, tft.width (), color1);
  }
  for (int16_t x=0; x < tft.width (); x+=5) {
    tft.drawFastVLine (x, 0, tft.height (), color2);
  }
}


void testdrawrects (uint16_t color) {
  tft.fillScreen (ST77XX_BLACK);
  for (int16_t x=0; x < tft.width (); x+=6) {
    tft.drawRect (tft.width ()/2 -x/2, tft.height ()/2 -x/2 , x, x, color);
  }
}


void testfillrects (uint16_t color1, uint16_t color2) {
  tft.fillScreen (ST77XX_BLACK);
  for (int16_t x=tft.width ()-1; x > 6; x-=6) {
    tft.fillRect (tft.width ()/2 -x/2, tft.height ()/2 -x/2 , x, x, color1);
    tft.drawRect (tft.width ()/2 -x/2, tft.height ()/2 -x/2 , x, x, color2);
  }
}


void testfillcircles (uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width (); x+=radius*2) {
    for (int16_t y=radius; y < tft.height (); y+=radius*2) {
      tft.fillCircle (x, y, radius, color);
    }
  }
}


void testdrawcircles (uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width ()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height ()+radius; y+=radius*2) {
      tft.drawCircle (x, y, radius, color);
    }
  }
}


void testtriangles() {
  tft.fillScreen (ST77XX_BLACK);
  uint16_t color = 0xF800;
  int t;
  int w = tft.width ()/2;
  int x = tft.height ()-1;
  int y = 0;
  int z = tft.width ();
  for (t = 0 ; t <= 15; t++) {
    tft.drawTriangle (w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}


void testroundrects () {
  tft.fillScreen (ST77XX_BLACK);
  uint16_t color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tft.width ()-2;
    int h = tft.height ()-2;
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect (x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}


void tftPrintTest () {
  tft.setTextWrap (false);
  tft.fillScreen (ST77XX_BLACK);
  tft.setCursor (0, 30);
  tft.setTextColor (ST77XX_RED);
  tft.setTextSize (1);
  tft.println ("Hello World!");
  tft.setTextColor (ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println ("Hello World!");
  tft.setTextColor (ST77XX_GREEN);
  tft.setTextSize (3);
  tft.println ("Hello World!");
  tft.setTextColor (ST77XX_BLUE);
  tft.setTextSize (4);
  tft.print (1234.567);
  delay (shortdelay*3);
  tft.setCursor (0, 0);
  tft.fillScreen (ST77XX_BLACK);
  tft.setTextColor (ST77XX_WHITE);
  tft.setTextSize (0);
  tft.println ("Hello World!");
  tft.setTextSize (1);
  tft.setTextColor (ST77XX_GREEN);
  tft.print (p, 6);
  tft.println (" Want pi?");
  tft.println (" ");
  tft.print (8675309, HEX); // print 8,675,309 out in HEX!
  tft.println (" Print HEX!");
  tft.println (" ");
  tft.setTextColor (ST77XX_WHITE);
  tft.println ("Sketch has been");
  tft.println ("running for: ");
  tft.setTextColor (ST77XX_MAGENTA);
  tft.print(millis () / 1000);
  tft.setTextColor (ST77XX_WHITE);
  tft.print (" seconds.");
}


void mediabuttons() {
  // play
  tft.fillScreen (ST77XX_BLACK);
  tft.fillRoundRect (25, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle (42, 20, 42, 60, 90, 40, ST77XX_RED);
  delay (shortdelay*2);
  // pause
  tft.fillRoundRect (25, 90, 78, 60, 8, ST77XX_WHITE);
  tft.fillRoundRect (39, 98, 20, 45, 5, ST77XX_GREEN);
  tft.fillRoundRect (69, 98, 20, 45, 5, ST77XX_GREEN);
  delay(shortdelay*2);
  // play color
  tft.fillTriangle (42, 20, 42, 60, 90, 40, ST77XX_BLUE);
  delay (shortdelay/10);
  // pause color
  tft.fillRoundRect (39, 98, 20, 45, 5, ST77XX_RED);
  tft.fillRoundRect (69, 98, 20, 45, 5, ST77XX_RED);
  // play color
  tft.fillTriangle (42, 20, 42, 60, 90, 40, ST77XX_GREEN);
}
