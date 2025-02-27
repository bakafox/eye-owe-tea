#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>

const char* FILE_WIFI = "/wifi.txt";
const char* FILE_SERVER = "/server.txt";
const char* FILE_PAGE = "/page.html";

ESP8266WebServer server(80); // Наш web-сервер, порт постоянный
WiFiClient client; // Для подключения к wifi-серверу другой ардуинки

int LED = D4; // Внутренний диодик (для отслеживания статуса)

// Данные wifi- сервера
String server_ip = "";
int server_port = 0;


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
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);

    // Ctrl+Shift+P --> "Upload LittleFS to ..."; Перед этим закрыть монитор порта!
    LittleFS.begin();
    if (!LittleFS.exists(FILE_WIFI) || !LittleFS.exists(FILE_PAGE) || !LittleFS.exists(FILE_SERVER)) {
        Serial.println("Files not found! Have you forgot to update LittleFS???");
        return;
    }

    // Получаем логин-пареоль сети
    File f = LittleFS.open(FILE_WIFI, "r");
    const int client_port = f.readStringUntil('\n').toInt();
    String wifi_ssid = f.readStringUntil('\n');
    String wifi_pass = f.readStringUntil('\n');
    f.close();

    // Увы, но "переоткрыть" web-сервер, в отличие от wifi-сервера, нельзя.
    // server = ESP8266WebServer(client_port);

    // Логинимся в сеть!
    Serial.print("Connecting to the network");

    WiFi.begin(wifi_ssid, wifi_pass);
    while (WiFi.status() != WL_CONNECTED) {
        if (WiFi.status() == WL_CONNECT_FAILED) {
            Serial.println("Connection Failed! Aborting.");
            return;
        }
        delay(500);
        Serial.print(".");
    }

    // Получаем айпишник и порт wifi-сервера
    f = LittleFS.open(FILE_SERVER, "r");
    server_ip = f.readStringUntil('\n');
    server_port = f.readStringUntil('\n').toInt();
    f.close();

    // Конфиг-я и запуск web-сервера
    server.on("/", HTTP_GET, getHtmlPage);
    server.on("/diod", HTTP_ANY, getDiodStatus);

    Serial.println("");
    Serial.println("Wi-Fi connected, starting web server..."); 
    server.begin();

    Serial.print("Success! Server address: ");
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


// Отправка html-странички клиенту при роуте "/"
void getHtmlPage() {
    digitalWrite(LED, LOW);
    File f = LittleFS.open(FILE_PAGE, "r");
    server.streamFile(f, "text/html");
    f.close();
    digitalWrite(LED, HIGH);
}

// При роуте "/diod", получаем или изменяем статус диода
void getDiodStatus() {
    String led = server.arg("led");
    String response = sendRequest(
        server.method() == HTTP_GET ? "GET" : "POST", led
    );
    server.send(200, "text/plain", response);
}


String sendRequest(String method, String led) {
    digitalWrite(LED, LOW);
    // Serial.println(server_ip);
    // Serial.println(server_port);

    if (client.connect(server_ip, server_port)) {
        // Отправляем этот запрос wifi-серверу
        client.print(
            method + " /" + led + " HTTP/1.1\r\n" +
            "Host: " + server_ip + "\r\n" +
            "Connection: close\r\n\r\n"
        );

        // Немного ждём, пока сервер подумает, и парсим ответ
        delay(100);
        String res = "";
        while (client.available()) {
            res += client.readStringUntil('\r');
        }
        client.stop();

        digitalWrite(LED, HIGH);
        return res;
    }

    digitalWrite(LED, HIGH);
    return "Connection failed!";
}
