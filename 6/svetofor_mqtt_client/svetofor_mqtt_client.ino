#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

const char* FILE_WIFI = "/wifi.txt";
const char* FILE_MQTT = "/mqtt.txt";

WiFiClient client; // (нужен для PubSubClient)
PubSubClient psc(client); // Для подключения к БРОКЕРУ сервера MQTT

int LED_RED = D1;
int LED_YELLOW = D2;
int LED_GREEN = D3;
int LED = D4; // Внутренний диодик (для отслеживания статуса)

// Данные для входа в брокер, не включая mqtt_id и mqtt_port:
String mqtt_topic;
String mqtt_user;
String mqtt_pass;


void setup() {
    Serial.begin(9600); // Монитор порта --> 9600 бод
    Serial.println("");

    // Устанавливаем все порты диодов на выход
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);

    // Ctrl+Shift+P --> "Upload LittleFS to ..."; Перед этим закрыть монитор порта!
    LittleFS.begin();
    if (!LittleFS.exists(FILE_WIFI) || !LittleFS.exists(FILE_MQTT)) {
        Serial.println("Required files not found! Have you forgotten to update LittleFS???");
        return;
    }

    // Получаем логин-пароль сети
    File f = LittleFS.open(FILE_WIFI, "r");
    const int client_port = f.readStringUntil('\n').toInt(); // Не исп.
    String wifi_ssid = f.readStringUntil('\n');
    String wifi_pass = f.readStringUntil('\n');
    f.close();

    // Логинимся в сеть
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

    // Получаем данные брокера MQTT-сервера
    f = LittleFS.open(FILE_MQTT, "r");
    String mqtt_ip = f.readStringUntil('\n');
    const int mqtt_port = f.readStringUntil('\n').toInt();
    mqtt_topic = f.readStringUntil('\n');
    mqtt_user = f.readStringUntil('\n');
    mqtt_pass = f.readStringUntil('\n');
    f.close();

    // Задаём параметры и подключаемся к брокеру
    psc.setServer(mqtt_ip.c_str(), mqtt_port);
    psc.setCallback(mqttOnMessage);
}


void loop() {
    if (!psc.connected()) {
        mqttSubscribe();
    }
    psc.loop();
}


void mqttOnMessage(char* topic, byte* payload, unsigned int length) {
    digitalWrite(LED, LOW);
    delay(50); // Оптимально от 20 до 200 мс, в зависимости от скорости сети

    // Парсим и применяем полученный ответ
    JsonDocument res;
    deserializeJson(res, payload);

    // Теперь этим управляет сервер, поэтому клиенту эту данные больше не отправляются:
    // const float duration = res["duration"];
    // state = res["next"];

    Serial.print("Received state ");
    Serial.print(String(res["state"]));
    Serial.print(" of ");
    Serial.print(String(res["mode"]));
    Serial.print(" mode!\n");

    // Проверки на NULL нужны, чтобы можно было отправлять неполный JSON, напр. { 'red': true }
    if (!res["red"].isNull()) {
        digitalWrite(LED_RED, res["red"]);
    }
    if (!res["yellow"].isNull()) {
        digitalWrite(LED_YELLOW, res["yellow"]);
    }
    if (!res["green"].isNull()) {
        digitalWrite(LED_GREEN, res["green"]);
    }
    digitalWrite(LED, HIGH);
}


void mqttSubscribe() {
    // Собственно, подписываемся на топик брокера
    Serial.print("Subscribing to MQTT \"");
    Serial.print(mqtt_topic.c_str());
    Serial.print("\" topic");

    while (!psc.connected()) {
        if (psc.connect("ESP8266Client", mqtt_user.c_str(), mqtt_pass.c_str())) {
            psc.subscribe(mqtt_topic.c_str(), 1);
            Serial.println("\nSuccess!");
        }
        else {
            Serial.print(' ');
            Serial.print(psc.state());
            if (psc.state() != -4) {
                delay(500); // Чтобы не дудосить брокера килотонной запросов
                psc.loop();
            }
        }
    }
}
