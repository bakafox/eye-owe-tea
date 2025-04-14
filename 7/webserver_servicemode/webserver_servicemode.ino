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

const int RESET = 0; // Внутренняя кнопка FLASH (да, оказывается, её можно программировать!)
const int LED = D4; // Внутренний диодик (им будем мигать при ошибке подключения к сети)

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


void loginDataSave() {
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");

    writeEEPROM(0, ssid, 32);
    writeEEPROM(32, pass, 64);
    EEPROM.commit();
    Serial.println("Updated EEOROM Wi-Fi data! Restaring...");

    delay(500);
    ESP.restart();
}


void startLoginMode(int ec) {
    blinkLast = 1;

    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP_Config");

    server.serveStatic("/login.html", LittleFS, "/login.html");

    // Делаем редирект, чтобы показать код ошибки на странице
    server.on("/", HTTP_GET, [ec]() {
        String redirectUrl = String("/login.html?error=") + String(ec);

        server.sendHeader("Location", redirectUrl);
        server.send(302, "text/plain", "Пожалуйста, подождите…");
    });

    // Для обработки отправленного запроса с новыми данными
    server.on("/save", HTTP_GET, loginDataSave);

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

        server.send(404, "text/plain", "Запрашиваемая страница не найдена!");
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
    Serial.println("");

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
        Serial.print("\nFailed to connect, error ");
        Serial.print(WiFi.status());
        Serial.println(". Entering Login Mode...");

        startLoginMode(WiFi.status());
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
