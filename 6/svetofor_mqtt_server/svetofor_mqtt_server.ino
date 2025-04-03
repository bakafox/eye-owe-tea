#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <map>
#include <vector>
#include <string>

const char* FILE_WIFI = "/wifi.txt";
const char* FILE_MQTT = "/mqtt.txt";
const char* FILE_STATES = "/states.ini";

WiFiClient client; // (нужен для PubSubClient)
PubSubClient psc(client); // Для подключения к БРОКЕРУ сервера MQTT

int LED = D4; // Внутренний диодик (для отслеживания статуса)
int STATE_CHANGE = D8; // Подаём питание --> состояние меняется

// Здесь храним данные состояний и режимов
std::map<std::string, std::vector<std::pair<int, float>>> states;
int state = 0;
int duration = 1000;
unsigned long timeSent = 0;
std::vector<std::string> modes;
bool modeChanged = false;
int modeIdx = 0;

// Данные для входа в брокер, не включая mqtt_id и mqtt_port:
String mqtt_topic;
String mqtt_user;
String mqtt_pass;


void setup() {
    Serial.begin(9600); // Монитор порта --> 9600 бод

    // Устанавливаем порты диодов
    pinMode(LED, OUTPUT);
    pinMode(STATE_CHANGE, INPUT);
    digitalWrite(LED, HIGH);

    // Ctrl+Shift+P --> "Upload LittleFS to ..."; Перед этим закрыть монитор порта!
    LittleFS.begin();
    if (!LittleFS.exists(FILE_WIFI) || !LittleFS.exists(FILE_STATES)) {
        Serial.println("Required files not found! Have you forgotten to update LittleFS???");
        return;
    }

    // Получаем логин-пароль сети
    File f = LittleFS.open(FILE_WIFI, "r");
    const int client_port = f.readStringUntil('\n').toInt(); // Не исп.
    String wifi_ssid = f.readStringUntil('\n');
    String wifi_pass = f.readStringUntil('\n');
    f.close();

    // Получаем состояния из файла (ахтунг, говнокод!)
    f = LittleFS.open(FILE_STATES, "r");
    std::string currState;
    while (f.available()) {
        String line = f.readStringUntil('\n');
        line.trim();

        if (line.startsWith(";") || line.isEmpty()) {
            continue;
        }
        else if (line.startsWith("[")) { 
            currState = std::string(line.substring(1, line.length() - 1).c_str());
            states[currState] = std::vector<std::pair<int, float>>();
            modes.push_back(currState);
        }
        else {
            int newSignal;
            float newDuration;
            if (sscanf(line.c_str(), "%d %f", &newSignal, &newDuration) == 2) {
                states[currState].emplace_back(newSignal, newDuration);
            }
        }
    }
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
}


std::pair<String, double> getStateData() {
    std::string currentMode = modes[modeIdx];

    const auto& stateSequence = states[currentMode];
    state = (state + stateSequence.size()) % stateSequence.size();

    int signal = stateSequence[state].first;
    double duration = stateSequence[state].second;

    JsonDocument resRaw;
    resRaw["mode"] = currentMode.c_str();
    resRaw["state"] = state;
    resRaw["red"] = (signal & 0b100) != 0;
    resRaw["yellow"] = (signal & 0b010) != 0;
    resRaw["green"] = (signal & 0b001) != 0;

    String res;
    serializeJson(resRaw, res);    

    Serial.print("State: ");
    Serial.print(res);
    Serial.print(", duration: ");
    Serial.println(duration);

    return { res, duration };
}


void loop() {
    if (digitalRead(STATE_CHANGE) == HIGH && !modeChanged) {
        delay(200); // Предотвращаем дребезг контактов!
                    // (не очень успешно, но лучше чем ничего)
        modeChanged = true;
        modeIdx = (modeIdx + 1) % modes.size();
    }

    unsigned long timeCurr = millis();
    if (timeCurr - timeSent >= duration) {
        timeSent = timeCurr;
        modeChanged = false;

        auto [stateStr, newDuration] = getStateData();
        duration = newDuration * 1000; // сек --> мс

        if (psc.connected()) {
            psc.publish(mqtt_topic.c_str(), stateStr.c_str());
        } else {
            mqttSubscribe();
        }
        state++;
    }

    psc.loop(); // Обработка входящих сообщений
}


void mqttSubscribe() {
    // Собственно, подписываемся на топик брокера
    Serial.print("Subscribing to MQTT \"");
    Serial.print(mqtt_topic.c_str());
    Serial.print("\" topic");

    while (!psc.connected()) {
        if (psc.connect("ESP8266Server", mqtt_user.c_str(), mqtt_pass.c_str())) {
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
