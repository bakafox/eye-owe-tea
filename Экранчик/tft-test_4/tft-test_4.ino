// Оригинальные статья и части кода по настройке дисплея (po polsku):
// https://forbiddenbit.com/arduino-projects/esp8266/nodemcu-and-1-8-spi-st7735-display/

#include <Adafruit_GFX.h>       
#include <Adafruit_ST7735.h> 
#include <SPI.h>
#include "FontsRus/Bahamas14.h"
#include "FontsRus/FreeSans6.h"


// Распиновка в порядке пинов на дисплее:
// VCC <--> 3V (питание 3.3 В)
// GND <--> G  (земля)
// CS  <--> D8 (GPIO15, HSPI_CS)
// RST <--> D3 (GPIO0)
// A0  <--> D4 (GPIO2)
// SCK <--> D5 (GPIO14, HSPI_SCK)
// SDA <--> D7 (GPIO13, HSPI_MOSI)
// LED <--> 3V (пиатние 3.3 В)

#define TFT_CS  15
#define TFT_RST 0
#define TFT_DC  2


const uint16_t colors[] = {
  ST77XX_RED, ST77XX_GREEN, ST77XX_BLUE,
  ST77XX_CYAN, ST77XX_MAGENTA, ST77XX_YELLOW, ST77XX_ORANGE
};
const size_t numColors = sizeof(colors) / sizeof(colors[0]);
unsigned int currBgColor = ST77XX_BLACK;
unsigned int currFgColor = ST77XX_YELLOW;

// Момент истины! Инициализируем дисплейчик... ГООООООЛ!!
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);


void setup() {
    // Ещё парочка небольших приготовлений
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);

    tft.setCursor(20, 90);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setFont(&FreeSans6pt8b);
    tft.print("- это просто космос!");  
} 


void loop() {
    // Выводим разноцветные текст и фоновый квадратик
    tft.fillRect(20, 40, 120, 30, currBgColor);
    tft.setCursor(25, 60);
    tft.setTextColor(currFgColor);
    tft.setFont(&Bahamas14pt8b);
    tft.print("Ардуино");

    currBgColor = colors[random(numColors)];
    do {
        currFgColor = colors[random(numColors)];
    } while (currBgColor == currFgColor);

    delay(200);
}
