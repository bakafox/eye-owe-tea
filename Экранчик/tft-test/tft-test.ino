#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>


// Порты дисплейчика
#define SCL D1
#define SDA D2
#define RES D3
#define DC  D4
#define CS  D8
// GND <--> G
// VCC <--> 3V
// BLK <--> 3V

// Размер дисплея: 240x240px
Adafruit_ST7789 display = Adafruit_ST7789(CS, DC, RES);


void setup() {
    Serial.begin(9600);
    Serial.println("");

    display.init(240, 240, SPI_MODE2);
    // display.setRotation(0);
}

void loop() {
    display.fillScreen(ST77XX_BLUE);
    delay(500);
    display.fillScreen(ST77XX_YELLOW);
    delay(500);
}
