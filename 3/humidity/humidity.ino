// https://randomnerdtutorials.com/esp8266-dht11dht22-temperature-and-humidity-web-server-with-arduino-ide/
#include <DHT.h> // Библиотека "DHT sensor library" (либо "SimpleDHT")

DHT dht(D2, DHT11); // Подключаем датчик влажности DHT11 (синенький такой) на порт D2

void setup() {
    Serial.begin(9600);
}

void loop() {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    Serial.printf("Температура, °C: %f\n", t);
    Serial.printf("Влажность, %%: %f\n", h);

    delay(400);
    Serial.println("");

    delay(100);
}
