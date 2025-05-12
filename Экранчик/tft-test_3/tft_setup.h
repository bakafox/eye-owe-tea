// Оригинальные статья и части кода по настройке дисплея: 
// https://www.makerguides.com/how-to-configure-tft_espi-library-for-tft-display/

#define ST7789_DRIVER // Собственно, наш дисплейчик

#define TFT_WIDTH  240
#define TFT_HEIGHT 240

#define TFT_RGB_ORDER TFT_BGR


// Почему именно такие пины -- см. в основном файле
#define TFT_MISO 12 // D6, он же GPIO12 // На дисплее -- НЕИСП
#define TFT_MOSI 13 // D7, он же GPIO13 // На диспрее -- SDA
#define TFT_SCLK 14 // D5, он же GPIO14 // На дисплее -- SCL
#define TFT_CS   15 // D8, он же GPIO15 // На дисплее -- НЕИСП

// Здесь можно любые порты, а не только порты SPI
#define TFT_RST  5  // D1, он же GPIO05 // На дисплее -- RES
#define TFT_DC   4  // D2, он же GPIO05 // На дисплее -- DC

#define TFT_BACKLIGHT_ON HIGH


#define LOAD_GLCD   
#define LOAD_FONT2  
#define LOAD_FONT4  
#define LOAD_FONT6
#define LOAD_FONT8  
#define LOAD_GFXFF
#define SMOOTH_FONT 

#define SPI_FREQUENCY  27000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000
