#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

const char* FILE_WIFI = "/wifi.txt";
const char* FILE_PAGE = "/cat_slim.html"; // <-- В 1.2.0 будет /cat_wide.html

const String FW_VERSION = "1.1.0";
const String FW_CHECK_URL = "https://eot.neocities.org/lab8/versions.json";
#define LED_BLINK_TIME 200 // <-- В 1.2.0 это значение будет увеличено до 800

#define PIN_LED D4 // Внутренний диодик
#define PIN_BTN 0  // Кнопка "FLASH"

ESP8266WebServer server(80); // Наш web-сервер для отображения кошака


void setup() {
    pinMode(PIN_BTN, INPUT);
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, HIGH);
    Serial.begin(9600);
    Serial.println("");

    // Ctrl+Shift+P --> "Upload LittleFS to ..."; Перед этим закрыть монитор порта!
    LittleFS.begin();
    if (!LittleFS.exists(FILE_WIFI) || !LittleFS.exists(FILE_PAGE)) {
        Serial.println("Required files not found! Have you forgotten to update LittleFS???");
        return;
    }

    // Получаем логин-пароль сети
    File f = LittleFS.open(FILE_WIFI, "r");
    const int client_port = f.readStringUntil('\n').toInt(); // Не исп.
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
        Serial.print('.');
        delay(500);
        attempts++;
    }
    if (WiFi.status() != WL_CONNECTED) {
        ESP.restart();
    }
    Serial.println("\nConnected!");

    // Конфигурация и запуск web-сервера
    Serial.println("Wi-Fi connected, starting web server..."); 
    server.on("/", HTTP_ANY, getHtmlPage);
    server.on("/cat.png", HTTP_ANY, getCatImage);
    server.begin();

    Serial.print("Success! Web server address: ");
    Serial.print(WiFi.localIP());
    Serial.println("");
}


unsigned long lastMillis = 0;
bool ledIsOn = false;

void loop() {
    unsigned long currMillis = millis();

    if (digitalRead(PIN_BTN) == LOW) {
        delay(500);
        checkForUpdates();
    }

    if (currMillis - lastMillis >= LED_BLINK_TIME) {
        lastMillis = currMillis;
        ledIsOn = !ledIsOn;
        digitalWrite(PIN_LED, ledIsOn ? LOW : HIGH);
    }

    server.handleClient();
}

void getHtmlPage() {
    digitalWrite(PIN_LED, LOW);

    File f = LittleFS.open(FILE_PAGE, "r");
    server.streamFile(f, "text/html");
    f.close();

    digitalWrite(PIN_LED, HIGH);
}

void getCatImage() {
    digitalWrite(PIN_LED, LOW);

    File file = LittleFS.open("/cat_slim.png", "r"); // <-- В 1.2.0 будет /cat_wide.png
    server.streamFile(file, "image/png");
    file.close();

    digitalWrite(PIN_LED, HIGH);
}


void checkForUpdates() {
    digitalWrite(PIN_LED, LOW);

    Serial.println("");
    Serial.print("Current firmware version: ");
    Serial.println(FW_VERSION);

    HTTPClient http;
    WiFiClientSecure client; // <-- С ПОДДЕРЖКОЙ TLS, ИНАЧЕ БУДЕТ ОШИБКА НОМЕР -5
    client.setInsecure(); // Вообще делать так очень небезопасно, но, увы, с ключом
                          // сертификата он не захотел работать от слова совсем...
    Serial.println("Checking for firmware updates...");
    http.begin(client, FW_CHECK_URL);

    int codeHttp = http.GET();
    if (codeHttp != 200) {
        Serial.print("Unable to get verions JSON! Errcode: ");
        Serial.println(codeHttp);

        http.end();
        return;
    }

    // Получаем и парсим JSON со списком версий
    String resRaw = http.getString();
    JsonDocument res;
    deserializeJson(res, resRaw);

    // Проверяем наличие более новых версий
    Serial.print("All available versions: ");
    int newestVerNum = 0;
    char newestVerIdx = 0;

    for (char i = 0; i < res.size(); i++) {
        Serial.print(res[i]["version"] + " ");
        int verNum = verToNum(res[i]["version"]);

        if (verNum > newestVerNum) {
            newestVerNum = verNum;
            newestVerIdx = i;
        }
    }
    Serial.println("");

    // Собственно, выполняем обновление, если нвдо
    if (newestVerNum > verToNum(FW_VERSION)) {
        Serial.print("New firmware version found: ");
        Serial.println(String(res[newestVerIdx]["version"]));

        Serial.print("Updating firmware via HTTP... ");
        HTTPUpdateResult codeUpdate = ESPhttpUpdate.update(
            client, res[newestVerIdx]["url"]
        );

        switch (codeUpdate) {
            case HTTP_UPDATE_FAILED:
                Serial.println(ESPhttpUpdate.getLastErrorString());
                break;

            case HTTP_UPDATE_OK:
                Serial.println("Success! Restarting...");
                ESP.restart();
        }
    }
    else {
        Serial.print("Your device is up to date: ");
        Serial.println(FW_VERSION);
    }

    http.end();
}

int verToNum(String ver) {
    int major = 0, minor = 0, patch = 0;
    sscanf(ver.c_str(), "%d.%d.%d", &major, &minor, &patch);
    return major*10000 + minor*100 + patch;
}
