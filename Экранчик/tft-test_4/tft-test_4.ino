// Оригинальные статья и части кода по настройке дисплея (po polsku):
// https://forbiddenbit.com/arduino-projects/esp8266/nodemcu-and-1-8-spi-st7735-display/

#include <Adafruit_GFX.h>       
#include <Adafruit_ST7735.h> 
#include <SPI.h>

// Распиновка на сей раз:
// VCC <--> 3V (питание 3.3 В)
// GND <--> G  (земля)
// CS  <--> D8 (GPIO15, HSPI_CS)
// RST <--> D3 (GPIO0)
// A0  <--> D4 (GPIO2)
// LED <--> 3V (пиатние 3.3 В)
// SCK <--> D5 (GPIO14, HSPI_SCK)
// DSA <--> D7 (GPIO13, HSPI_MOSI)

#define TFT_CS  15
#define TFT_RST 0
#define TFT_DC  2


void setup() {
    // Момент истины! Инициализируем дисплей... ГООООООЛ!!
    Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
    tft.initR(INITR_GREENTAB);

    tft.fillScreen(ST77XX_BLACK);
    tft.setTextWrap(false);
    tft.setCursor(45, 5);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_YELLOW);
    tft.print("USA");

    tft.setCursor(15, 25);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("COVID-19 Tracker");

    tft.setCursor(5, 45);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_BLUE);
    tft.print("Cases:");
    tft.setCursor(10, 65);
    tft.print("929,637");  

    tft.setCursor(5, 93);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_GREEN);
    tft.print("RECOVERED:");
    tft.setCursor(15, 115);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("110,504"); 

    tft.fillRect(0, 138 , 48, 10, ST77XX_WHITE); 
    tft.setCursor(7, 140);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_BLACK);
    tft.print("DEATHS:  ");
    tft.setCursor(55, 140);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("52,838");
} 


void loop() { }
