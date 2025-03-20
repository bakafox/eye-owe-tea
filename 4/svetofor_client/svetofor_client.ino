#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

const char* FILE_WIFI = "/wifi.txt";
const char* FILE_SERVER = "/server.txt";

WiFiClient client; // Для подключения к wifi-серверу другой ардуинки

int LED_RED = D1;
int LED_YELLOW = D2;
int LED_GREEN = D3;
int LED = D4; // Внутренний диодик (для отслеживания статуса)

// Данные wifi- сервера
String server_ip = "";
int server_port = 0;
int state = 0;


void setup() {
    Serial.begin(9600); // Монитор порта -> 9600 бод
    Serial.println("");

    // Устанавалиаем все порты диодов на выход
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);

    // Ctrl+Shift+P --> "Upload LittleFS to ..."; Перед этим закрыть монитор порта!
    LittleFS.begin();
    if (!LittleFS.exists(FILE_WIFI) || !LittleFS.exists(FILE_SERVER)) {
        Serial.println("Required files not found! Have you forgot to update LittleFS???");
        return;
    }

    // Получаем логин-пареоль сети
    File f = LittleFS.open(FILE_WIFI, "r");
    const int client_port = f.readStringUntil('\n').toInt();
    String wifi_ssid = f.readStringUntil('\n');
    String wifi_pass = f.readStringUntil('\n');
    f.close();

    // Логинимся в сеть!
    Serial.print("\nConnecting to \"");
    Serial.print(wifi_ssid.c_str());
    Serial.print("\" Wi-Fi");

    WiFi.begin(wifi_ssid, wifi_pass);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() != WL_CONNECTED) {
        ESP.restart();
    }
    Serial.println("Success!");

    // Получаем айпишник и порт wifi-сервера
    f = LittleFS.open(FILE_SERVER, "r");
    server_ip = f.readStringUntil('\n');
    server_port = f.readStringUntil('\n').toInt();
    f.close();
}


void loop() {
    if (!client.connect(server_ip.c_str(), server_port)) {
        return;
    }

    String request = "GET /" + String(state) + " HTTP/1.1\r\nHost: " + server_ip + "\r\nConnection: close\r\n\r\n";
    client.print(request);
    delay(100);

    if (!client.available()) { return; }
    String resRaw = "";
    // Перезаписываем хэдеры и получаем только JSON
    while (client.available()) {
        resRaw = client.readStringUntil('\r\n');
    }
    client.stop();
    Serial.println(resRaw);

    digitalWrite(LED, LOW);
    JsonDocument res;
    deserializeJson(res, resRaw);

    const float d = res["duration"];
    state = res["next"];

    digitalWrite(LED_RED, res["red"]);
    digitalWrite(LED_YELLOW, res["yellow"]);
    digitalWrite(LED_GREEN, res["green"]);
    
    digitalWrite(LED, HIGH);
    delay(d*1000.0); // сек --> мс
}
