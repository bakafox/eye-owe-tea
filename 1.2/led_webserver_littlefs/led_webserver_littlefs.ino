// https://iotdesignpro.com/projects/esp8266-based-webserver-to-control-led-from-webpage
// https://github.com/earlephilhower/arduino-littlefs-upload (СМ. README!!!)
#include <ESP8266WiFi.h>
#include <LittleFS.h>

const char* FILE_WIFI = "/wifi.txt";
// const char* FILE_LOGS = "/logs.txt";

WiFiServer server(24680); // Временное значение порта на момент инициализации!

// Имя "LED", походу, используется встроенным светодиодом, и это и вызывало конфликт!
int MY_LED = D3; // Номер порта (D3, согласно распиновке, это GPIO0)
int LED = D4;


// В этой гаўне даже аналога ls нет! Лооооол...
// Реализация взята отсюда: https://github.com/littlefs-project/littlefs/issues/2
void listAllFilesInDir(String dir_path) {
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


// Особая функция для Arduino, сработает при первом запуске!
// Здесь мы подключаемся к Wi-Fi и запускаем на нём свой сервер
void setup() {
    Serial.begin(9600); // Монитор порта -> 9600 бод
    // (значение по умолчанию, можно и другое использовать)

    pinMode(MY_LED, OUTPUT); // Устанавалиаем режим порта на выход
    pinMode(LED, OUTPUT);

    // Почему в Ордуине такая всратая работа с ФС?
    LittleFS.begin();
    listAllFilesInDir("/");

    if (!LittleFS.exists(FILE_WIFI)) {
        // Ctrl+Shift+P --> "Upload LittleFS to ..."; Перед этим закрыть монитор порта!
        Serial.println("FILE_WIFI not found! Have you forgot to update LittleFS???");
        return;
    }
    File f = LittleFS.open(FILE_WIFI, "r");

    const int server_port = f.readStringUntil('\n').toInt();
    String wifi_ssid = f.readStringUntil('\n');//.c_str();
    String wifi_pass = f.readStringUntil('\n');//.c_str();
    Serial.println(server_port);
    Serial.println(wifi_ssid);
    Serial.println(wifi_pass);

    server = WiFiServer(server_port); // Переоткрываем сервер на нужном порту
    Serial.print("Connecting to the network");
    WiFi.begin(wifi_ssid, wifi_pass); // Логинимся в сеть!

    while (WiFi.status() != WL_CONNECTED) {
        if (WiFi.status() == WL_CONNECT_FAILED) {
            Serial.println("Connection Failed! Aborting.");
            return;
        }
        delay(500);
        Serial.print(".");
    }


    Serial.println("");
    Serial.println("Wi-Fi connected, starting server..."); 
    server.begin(); // Запуск сервера

    Serial.print("Success! Copy and paste the following URL: ");
    Serial.print(WiFi.localIP());
    Serial.print(":");
    Serial.print(server_port);
    Serial.println("");
}


// Особая функция для Arduino, работает в бесконечном цикле!
// Тут мы отправляем веб-страницу клиенту и управляем состоянием светодиода
void loop() {
    WiFiClient client = server.available();

    if (!client) { return; }

    Serial.println("Waiting for new client");
    while(!client.available()) { delay(1); }

    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();


    int value = LOW; // LOW -- низкое напряжение (по сути выкл)

    if (request.indexOf("/LED=ON") != -1) {
        value = HIGH; // HIGH -- высокое напряжение (по сути вкл)
    }
    if (request.indexOf("/LED=OFF") != -1) {
        value = LOW; // LOW -- низкое напряжение (по сути выкл)
    }

    digitalWrite(MY_LED, value); // Включаем/Выключаем светодиод
    digitalWrite(LED, !value);


    client.println("HTTP/1.1 200 OK"); // Стартовая строка
    client.println("Content-Type: text/html; charset=utf-8"); // Формат и кодировка
    client.println(""); // Пустая строка отделяет тело сообщения

    client.println("<!DOCTYPE HTML>"); // Далее и до конца, собственно, HTML-код
    client.println("<html>");

    client.print(" Состояние питания светодиода: ");
    if (value == HIGH) { client.print("<b>ВКЛ</b>"); }
    else { client.print("<b>ВЫКЛ</b>"); }

    client.println("<br><br>");
    client.println("<a href=\"/LED=ON\"\"><button>Включить</button></a>");
    client.println("<a href=\"/LED=OFF\"\"><button>Выключить</button></a><br />");
    client.println("</html>");

    delay(1);
    Serial.println("Client disonnected");
    Serial.println("");
}
