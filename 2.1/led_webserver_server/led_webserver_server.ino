#include <ESP8266WiFi.h>
#include <LittleFS.h>

const char* FILE_WIFI = "/wifi.txt";

WiFiServer server(24680); // Временное значение порта на момент инициализации!

int LED_RED = D1; // = GPIO5
int LED_GREEN = D2; // = GPIO4
int LED = D4; // Внутренний диодик (для отслеживания статуса)


void listAllFilesInDir(String dir_path) {
    // Взято отсюда: https://github.com/littlefs-project/littlefs/issues/2
	  Dir dir = LittleFS.openDir(dir_path);

	  while(dir.next()) {
        if (dir.isFile()) {
            Serial.print("File: ");
            Serial.println(dir_path + dir.fileName());
        }
        if (dir.isDirectory()) {
            Serial.print("Dir: ");
            Serial.println(dir_path + dir.fileName() + "/");
            listAllFilesInDir(dir_path + dir.fileName() + "/");
        }
    }
}


void setup() {
    Serial.begin(9600); // Монитор порта -> 9600 бод
    Serial.println("");

    // Устанавалиаем все порты диодов на выход
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);

    // Ctrl+Shift+P --> "Upload LittleFS to ..."; Перед этим закрыть монитор порта!
    LittleFS.begin();
    if (!LittleFS.exists(FILE_WIFI)) {
        Serial.println("FILE_WIFI not found! Have you forgot to update LittleFS???");
        return;
    }
    File f = LittleFS.open(FILE_WIFI, "r");

    const int server_port = f.readStringUntil('\n').toInt();
    String wifi_ssid = f.readStringUntil('\n');
    String wifi_pass = f.readStringUntil('\n');

    // Переоткрываем сервер на нужном порту
    server = WiFiServer(server_port);
    Serial.print("Connecting to the network");

    // Логинимся в сеть!
    WiFi.begin(wifi_ssid, wifi_pass);
    while (WiFi.status() != WL_CONNECTED) {
        if (WiFi.status() == WL_CONNECT_FAILED) {
            Serial.println("Connection Failed! Aborting.");
            return;
        }
        delay(500);
        Serial.print(".");
    }

    // Запуск сервера
    Serial.println("");
    Serial.println("Wi-Fi connected, starting wifi server..."); 
    server.begin();

    Serial.print("Success! Server address: ");
    Serial.print(WiFi.localIP());
    Serial.print(":");
    Serial.print(server_port);
    Serial.println("");
}


void loop() {
    WiFiClient client = server.available();
    if (!client) { return; }

    // Получен новый запрос!
    String request = client.readStringUntil('\r');
    Serial.println("");
    Serial.println(request);

    digitalWrite(LED, LOW);
    client.flush();

    // GET-запрос -- просто возвращаем текущий статус диода
    if (request.startsWith("GET")) {
        if (request.indexOf("/red") != -1) {
            send200(client, digitalRead(LED_RED));
        }
        else if (request.indexOf("/green") != -1) {
            send200(client, digitalRead(LED_GREEN));
        }
        else {
            send404(client);
        }
    }
    // POST-запрос -- переключакм статус диода и возвращаем его
    else if (request.startsWith("POST")) {
        if (request.indexOf("/red") != -1) {
            digitalWrite(LED_RED, !digitalRead(LED_RED));
            send200(client, digitalRead(LED_RED));
        }
        else if (request.indexOf("/green") != -1) {
            digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
            send200(client, digitalRead(LED_GREEN));
        }
        else {
            send404(client);
        }
    }
    // Все остальные запросы -- возвращаем ошибку
    else {
        send404(client);
    }
    // Проверить работу сервера можно и без клиента --
    // например, используя утилиты "Bruno" или "Insomnia".

    // Обработка запроса завершена
    delay(100);
    digitalWrite(LED, HIGH);
}


void send200(WiFiClient& client, int status) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("");
    client.println(status == HIGH ? "ON" : "OFF");
}

void send404(WiFiClient& client) {
    client.println("HTTP/1.1 404 Not Found");
    client.println("Content-Type: text/plain");
    client.println("");
    client.println("This diod does not exist!");
}
