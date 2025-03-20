#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <map>
#include <vector>
#include <string>

const char* FILE_WIFI = "/wifi.txt";
const char* FILE_STATES = "/states.ini";

WiFiServer server(24680); 

int LED = D4; 
int STATE_CHANGE = D8;
std::map<std::string, std::vector<std::pair<int, float>>> states;
std::vector<std::string> modes;
bool modeChanged = false;
int modeIdx = 0;


void setup() {
    Serial.begin(9600);
    pinMode(LED, OUTPUT);
    pinMode(STATE_CHANGE, INPUT);
    digitalWrite(LED, HIGH);

    LittleFS.begin();
    if (!LittleFS.exists(FILE_WIFI) || !LittleFS.exists(FILE_STATES)) {
        Serial.println("Required files not found! Have you forgot to update LittleFS???");
        return;
    }
    File f;

    f = LittleFS.open(FILE_WIFI, "r");
    const int server_port = f.readStringUntil('\n').toInt();
    String wifi_ssid = f.readStringUntil('\n');
    String wifi_pass = f.readStringUntil('\n');
    f.close();

    f = LittleFS.open(FILE_STATES, "r");
    if (!f) {
        Serial.println("Failed to open FILE_STATES!");
        return;
    }

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

    server = WiFiServer(server_port);
    Serial.println("\nWi-Fi connected, starting server...");
    server.begin();

    Serial.print("Success! Server URL: ");
    Serial.print(WiFi.localIP());
    Serial.print(":");
    Serial.print(server_port);
    Serial.println("");
}


String getStateData(int state) {
    std::string currentMode = modes[modeIdx];

    const auto& stateSequence = states[currentMode];
    if (state < 0 || state >= stateSequence.size()) {
        state = 0; // Если индекс некорректный, сбрасываем в 0
    }

    int signal = stateSequence[state].first;
    double duration = stateSequence[state].second;

    JsonDocument resRaw;
    resRaw["mode"] = currentMode.c_str();
    resRaw["red"] = (signal & 0b100) != 0;
    resRaw["yellow"] = (signal & 0b010) != 0;
    resRaw["green"] = (signal & 0b001) != 0;
    resRaw["duration"] = duration;
    resRaw["next"] = (state + 1) % stateSequence.size();

    String res;
    serializeJson(resRaw, res); 
    Serial.println(res);
    return res;
}


void loop() {
    if (digitalRead(STATE_CHANGE) == HIGH && !modeChanged) {
        delay(200); // Предотвращаем дребезг контактов!
                    // (не очень успешно, но лучше чем ничего)
        modeChanged = true;

        modeIdx = (modeIdx + 1) % modes.size();  // Переключаем режим
        Serial.println("Mode changed to: " + String(modes[modeIdx].c_str()));
    }
    else if (digitalRead(STATE_CHANGE) == LOW) {
        modeChanged = false;
    }

    WiFiClient client = server.available();
    if (!client) return;

    digitalWrite(LED, HIGH);
    String req = client.readStringUntil('\r'); // Читаем первую строку запроса
    client.flush(); // Очищаем буфер после чтения

    int startIdx = req.indexOf("/");
    int endIdx = req.indexOf("HTTP");
    if (startIdx == -1 || endIdx == -1) {
        client.println("HTTP/1.1 400 Bad Request");
        client.println("Content-Type: application/json");
        client.println();
        client.println("{\"error\": \"Invalid request\"}");
        client.stop();

        digitalWrite(LED, LOW);
        return;
    }
    String stateStr = req.substring(startIdx+1, endIdx-1);
    int state = stateStr.toInt();

    String res = getStateData(state);
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.println(res);

    Serial.println("Request done!\n");
    digitalWrite(LED, LOW);
}
