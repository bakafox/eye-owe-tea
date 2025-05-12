// ST_7789_240x240TFT_ESP8266_most simple
// Floris Wouterlood 21 December 2019
// public domain
// sketch for NodeMCU ESP8266
// displays HELLO WORLD on a 1.3 inch 3.3V TFT_display with ST7789 controller
// 240 x 240 pixels in this confguration

#include <Arduino_ST7789.h> // hardware-specific library for ST7789
#include <SPI.h>


#define TFT_DC D1 // pin of your choice
#define TFT_RST D0 // pin of your choice
#define TFT_MOSI D7 // fixed pin
#define TFT_SCLK D5 // fixed pin

Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST);


void setup(void) {
    Serial.begin(9600);
    Serial.println(); // cut the gibberish first line Serial Monitor
    Serial.println();
    Serial.print("Привет, мир!"); // test Serial Monitor

    tft.init (240, 240); // initialize a ST7789 chip, 240×240 pixels
    tft.fillScreen (BLACK);
    tft.setCursor (20, 50);
    tft.setTextColor (YELLOW);
    tft.setTextSize (3);
    tft.println("ПРИВЕТ, МИР!"); // initialization message on the TFT display
    tft.println ("................");
}


void loop () {

}
