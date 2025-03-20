// https://iotdesignpro.com/projects/esp8266-based-webserver-to-control-led-from-webpage
#include <ESP8266WiFi.h>

const int SERVER_PORT = 9612;
WiFiServer server(SERVER_PORT); // Класс нашего Wi-Fi сервера на порту 9612
// "РЯЯ СПАЛИЛ ЛОГИНПАРОЛЬ" -- Да пофиг уже, будете мимо ВЦ проходить подключайтесь, роутер-то не мой :)
const char* WIFI_SSID = "IIR_WiFi";   
const char* WIFI_PASS = "deeprobotics";

// Имя "LED", походу, используется встроенным светодиодом, и это и вызывало конфликт!
int MY_LED = D3; // Номер порта (D3, согласно распиновке, это GPIO0)


// Особая функция для Arduino, сработает при первом запуске!
// Здесь мы подключаемся к Wi-Fi и запускаем на нём свой сервер
void setup() {
    Serial.begin(9600); // Монитор порта -> 9600 бод
    // (значение по умолчанию, можно и другое использовать)

    pinMode(MY_LED, OUTPUT); // Устанавалиаем режим порта на выход

    Serial.print("Connecting to the Network");
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }


    Serial.println("");
    Serial.println("Wi-Fi connected"); 
    server.begin(); // Запуск сервера
    Serial.println("Server started");

    Serial.print("Copy and paste the following URL: ");
    Serial.print(WiFi.localIP());
    Serial.print(":");
    Serial.print(SERVER_PORT);
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
