#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <LittleFS.h>

ESP8266WebServer server;

const char* FILE_FUNNI = "/funni.html";
const char* FILE_LOGIN = "/login.html";

// Длина, занимаемая каждым из полей в EEPROM
const int EEPROM_SIZE = 512;
const int EEPROM_SSID = 32;
const int EEPROM_PASS = 64;

const int RESET = 0; // Сюда прилетает сигнал от кнопки
const int LED = D4; // Внутренний диодик (для отслеживания статуса)

unsigned long blinkLast = 1;
bool blinkIsOn = false;


// https://github.com/ionciubotaru/ESP8266-Easy-config/blob/a7403f4b3dcea7dc602af6bcc2b819b0e3c39f86/functions.ino#L76
String readEEPROM(int offset, int length) {
    String data = "";
    for (int i = 0; i < length; i++) {
        char c = EEPROM.read(offset + i);
        data += c;
    }
    return data;
}

// https://github.com/ionciubotaru/ESP8266-Easy-config/blob/a7403f4b3dcea7dc602af6bcc2b819b0e3c39f86/functions.ino#L85
void writeEEPROM(int offset, const String& data, int length) {
    for (int i = 0; i < length; i++) {
        if (i < data.length()) {
            EEPROM.write(offset + i, data[i]);
        }
        else EEPROM.write(offset + i, 0);
    }
}


void handleSave() {
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");

    if (ssid.length() > 0) {
        server.sendHeader("Location", "/login.html?success=1", true);
        server.send(302, "text/plain", "");
        delay(1500);

        writeEEPROM(0, ssid, 32);
        writeEEPROM(32, pass, 64);
        EEPROM.commit();
        Serial.println("Updated EEOROM Wi-Fi data! Restaring...");

        delay(500);
        ESP.restart();
    }
    else {
        server.sendHeader("Location", "/login.html?error=1", true);
        server.send(302, "text/plain", "");
    }
}


void startLoginMode() {
    blinkLast = 1;

    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP_Config");

    server.serveStatic("/", LittleFS, FILE_LOGIN);
    server.serveStatic(FILE_LOGIN, LittleFS, FILE_LOGIN);

    server.on("/save", HTTP_GET, handleSave);

    server.begin();
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
}


void startFunniMode() {
    blinkLast = 0;

    server.serveStatic("/", LittleFS, FILE_FUNNI);
    server.serveStatic(FILE_FUNNI, LittleFS, FILE_FUNNI);

    server.onNotFound([]() {
        digitalWrite(LED, LOW);
        delay(100);
        digitalWrite(LED, HIGH);

        server.send(404, "text/plain", "Not found");
    });

    server.begin();
    Serial.print("STA IP: ");
    Serial.print(WiFi.localIP().toString().c_str());
}


void setup() {
    Serial.begin(9600);
    EEPROM.begin(EEPROM_SIZE);
    LittleFS.begin();

    // Устанавливаем порты
    pinMode(RESET, INPUT);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    Serial.printf("");

    // Данные для точки доступа
    String wifi_ssid = readEEPROM(0, 32);
    String wifi_pass = readEEPROM(32, 64);
    WiFi.mode(WIFI_STA);

    // Логинимся в сеть
    Serial.print("\nConnecting to \"");
    Serial.print(wifi_ssid.c_str());
    Serial.print("\" Wi-Fi");
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());

    int tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries++ < 20) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected successfully... To the Funni Mode we go!");

        startFunniMode();
    }
    else {
        Serial.println("\nFailed to connect to Wi-Fi. Entering Login Mode...");

        startLoginMode();
    }
}

void loop() {
    server.handleClient();

    // Кнопка сброса EEPROM-а
    if (digitalRead(RESET) == LOW) {
        for (int i = 0; i < EEPROM_SIZE; i++) {
            
            EEPROM.write(i, 0);
        }
        EEPROM.commit();

        delay(500);
        ESP.restart();
    }

    // Мигалка для Login Mode
    if (blinkLast != 0) {
        unsigned long now = millis();

        if (now - blinkLast >= 300) {
            blinkIsOn = !blinkIsOn;
            digitalWrite(LED, blinkIsOn ? LOW : HIGH);
            blinkLast = now;
        }
    }
}
