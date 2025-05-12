// ESP8266_ST7735_Bodmers_clock_nn
// microcontroller: ESP8266
// display: 8-pin 128*160 SPI TFT (SD card reader not used) controller ST7735S
// Example in the eTFT_SPI library
// by Bodmer
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

// public domain
// Floris Wouterlood
// January 1, 2023

// ================================================================
/*
 An example analogue clock using a TFT LCD screen to show the time
 use of some of the drawing commands with the ST7735 library.

 For a more accurate clock, it would be better to use the RTClib library.
 But this is just a demo. 

 Uses compile time to set the time so a reset will start with the compile time again
 
 Gilchrist 6/2/2014 1.0
 Updated by Bodmer
 */


#include <TFT_eSPI.h>                                     // graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();                                // invoke library, pins defined in User_Setup.h

#define TFT_GREY 0xBDF7
#define DEG2RAD 0.0174532925

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // saved H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=64, osy=64, omx=64, omy=64, ohx=64, ohy=64;  // saved H, M, S x & y coords
uint16_t x0=0, x1=0, yy0=0, yy1=0;
uint32_t targetTime = 0;                                  // for next 1 second timeout

static uint8_t conv2d (const char* p) {
   uint8_t v = 0;
   if ('0' <= *p && *p <= '9')
      v = *p - '0';
   return 10 * v + *++p - '0';
}

uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);  // Get H, M, S from compile time

bool initial = 1;


void setup(void) {
   tft.init ();
   tft.setRotation (0);
   tft.fillScreen (TFT_GREY);
   tft.setTextColor (TFT_GREEN, TFT_GREY);                   // adding a black background colour erases previous text automatically
  
   // Draw clock face
   tft.fillCircle (64, 64, 61, TFT_BLUE);
   tft.fillCircle (64, 64, 57, TFT_BLACK);

   // Draw 12 lines
   for (int i = 0; i<360; i+= 30)
      {
       sx = cos ((i-90)*DEG2RAD);
       sy = sin ((i-90)*DEG2RAD);
       x0 = sx*57+64;
       yy0 = sy*57+64;
       x1 = sx*50+64;
       yy1 = sy*50+64;
       tft.drawLine (x0, yy0, x1, yy1, TFT_BLUE);
      }

   // Draw 60 dots
   for (int i = 0; i<360; i+= 6)
      {
       sx = cos ((i-90)*DEG2RAD);
       sy = sin ((i-90)*DEG2RAD);
       x0 = sx*53+64;
       yy0 = sy*53+64;
       tft.drawPixel (x0, yy0, TFT_BLUE);   
       if(i==0  || i==180) tft.fillCircle (x0,   yy0, 1, TFT_CYAN);
       if(i==0  || i==180) tft.fillCircle (x0+1, yy0, 1, TFT_CYAN);
       if(i==90 || i==270) tft.fillCircle (x0,   yy0, 1, TFT_CYAN);
       if(i==90 || i==270) tft.fillCircle (x0+1, yy0, 1, TFT_CYAN);
      }

   tft.fillCircle (65, 65, 3, TFT_RED);

   // draw text at position 64,125 using fonts 4
   // only font numbers 2,4,6,7 are valid. Font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : . a p m
   // font 7 is a 7 segment font and only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : .
   tft.drawCentreString ("Time flies",64,130,4);
   targetTime = millis () + 1000; 
}


void loop() {
   if (targetTime < millis())
      {
       targetTime = millis()+1000;
       ss++;                                                 // advance second
         if (ss==60)
            {
             ss=0;
             mm++;                                           // advance minute
             if (mm>59)
                {
                 mm=0;
                 hh++;                                       // advance hour
                 if (hh>23)
                    {
                     hh=0;
                    }
                }
            }

       // pre-compute hand degrees, x & y coords for a fast screen update
       sdeg = ss*6;                                          // 0-59 -> 0-354
       mdeg = mm*6+sdeg*0.01666667;                          // 0-59 -> 0-360 - includes seconds
       hdeg = hh*30+mdeg*0.0833333;                          // 0-11 -> 0-360 - includes minutes and seconds
       hx = cos ((hdeg-90)*DEG2RAD);    
       hy = sin ((hdeg-90)*DEG2RAD);
       mx = cos ((mdeg-90)*DEG2RAD);    
       my = sin ((mdeg-90)*DEG2RAD);
       sx = cos ((sdeg-90)*DEG2RAD);    
       sy = sin ((sdeg-90)*DEG2RAD);

       if (ss==0 || initial)
         {
          initial = 0;
          // erase hour and minute hand positions every minute
          tft.drawLine (ohx, ohy, 65, 65, TFT_BLACK);
          ohx = hx*33+65;    
          ohy = hy*33+65;
          tft.drawLine (omx, omy, 65, 65, TFT_BLACK);
          omx = mx*44+65;    
          omy = my*44+65;
         }

       // redraw new hand positions, hour and minute hands not erased here to avoid flicker
       tft.drawLine (osx, osy, 65, 65, TFT_BLACK);
       tft.drawLine (ohx, ohy, 65, 65, TFT_WHITE);
       tft.drawLine (omx, omy, 65, 65, TFT_WHITE);
       osx = sx*47+65;    
       osy = sy*47+65;
       tft.drawLine (osx, osy, 65, 65, TFT_RED);
       tft.fillCircle (65, 65, 3, TFT_RED);
      }
}
