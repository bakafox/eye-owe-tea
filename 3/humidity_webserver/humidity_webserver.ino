#include <LittleFS.h> // Читать и записывать файлы во внутреннюю память
#include <ESP8266WebServer.h> // Для открытия веб-сервера
#include <DHT.h> // Библиотека "DHT sensor library" (либо "SimpleDHT")
#include <ArduinoJson.h> // Чтение и запись JSON-файлов

const char* FILE_WIFI = "/wifi.txt";
const char* FILE_PAGE = "/page.html";

ESP8266WebServer server(80); // Наш web-сервер, порт постоянный

// https://randomnerdtutorials.com/esp8266-dht11dht22-temperature-and-humidity-web-server-with-arduino-ide/
DHT dht(D2, DHT11); // Подключаем датчик влажности DHT11 (синенький такой) на порт D2

int LED = D4; // Внутренний диодик (для отслеживания статуса)

// Данные wifi-сервера
String server_ip = "";
int server_port = 0;


void setup() {
    Serial.begin(9600); // Монитор порта -> 9600 бод
    Serial.println("");

    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);

    // Ctrl+Shift+P --> "Upload LittleFS to ..."; Перед этим закрыть монитор порта!
    LittleFS.begin();
    if (!LittleFS.exists(FILE_WIFI) || !LittleFS.exists(FILE_PAGE)) {
        Serial.println("Files not found! Have you forgot to update LittleFS???");
        return;
    }

    // Получаем логин-пароль сети
    File f = LittleFS.open(FILE_WIFI, "r");
    const int client_port = f.readStringUntil('\n').toInt();
    String wifi_ssid = f.readStringUntil('\n');
    String wifi_pass = f.readStringUntil('\n');
    f.close();

    // Увы, но "переоткрыть" web-сервер, в отличие от wifi-сервера, нельзя.
    // server = ESP8266WebServer(client_port);

    // Логинимся в сеть!
    Serial.print("Connecting to \"");
    Serial.print(wifi_ssid.c_str());
    Serial.print("\" Wi-Fi");

    WiFi.begin(wifi_ssid, wifi_pass);
    while (WiFi.status() != WL_CONNECTED) {
        if (WiFi.status() == WL_CONNECT_FAILED) {
            Serial.println("Connection Failed! Aborting.");
            return;
        }
        delay(500);
        Serial.print(".");
    }

    // Конфиг-я и запуск web-сервера
    server.on("/", HTTP_GET, getPageHtml);
    server.on("/temperature", HTTP_ANY, getTemperatureJson);
    server.on("/humidity", HTTP_ANY, getHumidityJson);

    Serial.println("");
    Serial.println("Wi-Fi connected, starting web server..."); 
    server.begin();

    Serial.print("Success! Web server address: ");
    Serial.print(WiFi.localIP());
    // Serial.print(":");
    // Serial.print(client_port);
    Serial.println("");
}


void loop() {
    // В библиотеке web server-а мы отдельно выносим функции
    // для обработки каждого из роутов, а тут просто запускаем её:
    server.handleClient();
}


void getPageHtml() {
    digitalWrite(LED, LOW);

    File f = LittleFS.open(FILE_PAGE, "r");
    server.streamFile(f, "text/html");
    f.close();

    digitalWrite(LED, HIGH);
}

void getTemperatureJson() {
    digitalWrite(LED, LOW);

    // String led = server.arg("led");
    float t = dht.readTemperature();
    unsigned long timestamp = millis();

    // Преобразуем в JSON и отправляем
    JsonDocument resRaw;
    resRaw["temperature"] = t * 1.0;
    resRaw["lastUpdated"] = millis();

    String res = "";
    serializeJsonPretty(resRaw, res);
    Serial.println(res);
    server.send(200, "application/json", res);

    digitalWrite(LED, HIGH);
}

void getHumidityJson() {
    digitalWrite(LED, LOW);

    // String led = server.arg("led");
    float h = dht.readHumidity();
    unsigned long timestamp = millis();

    // Преобразуем в JSON и отправляем
    JsonDocument resRaw;
    resRaw["humidity"] = h * 1.0;
    resRaw["lastUpdated"] = millis();

    String res = "";
    serializeJsonPretty(resRaw, res);
    Serial.println(res);
    server.send(200, "application/json", res);

    digitalWrite(LED, HIGH);
}
