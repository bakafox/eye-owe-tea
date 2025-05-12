// Оригинальные статья и части кода по настройке дисплея: 
// https://www.makerguides.com/how-to-configure-tft_espi-library-for-tft-display/

#include "tft_setup.h"
#include"TFT_eSPI.h"

TFT_eSPI tft = TFT_eSPI();

const int cw = tft.width()/2;
const int ch = tft.height()/2;
const int s = min(cw/4,ch/4);


void setup() {
    Serial.begin(115200);
    Serial.println("");

    // Код для сверки правильности пинов с дефайнами...
    Serial.print("HSPI MISO -- ");
    Serial.println(MISO);
    Serial.print("HSPI MOSI -- ");
    Serial.println(MOSI);
    Serial.print("HSPI SCLK -- ");
    Serial.println(SCK);
    Serial.print("HSPI CS -- ");
    Serial.println(SS);


    tft.init();
    tft.fillScreen(TFT_BLACK);  
    tft.setRotation(1);

    tft.setTextFont(1);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(CC_DATUM);
    tft.drawString("Makerguides", ch, cw+s);

    tft.fillCircle(ch, cw/2+s/2, s/2, TFT_RED);
    tft.fillRect(1.5*ch-s, cw/2, s, s, TFT_GREEN);
    tft.fillTriangle(ch/2, cw/2, ch/2+s, cw/2, ch/2, cw/2+s, TFT_BLUE);
}

void loop() { }
