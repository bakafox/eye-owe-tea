// Функция setup() срабатывает один раз, при подключении или перезагрузке!
void setup() {
    // Инициализируем режим порта встроенного светодиода на выход
    pinMode(LED_BUILTIN, OUTPUT);
}

int waitTime = 300;

// Функуия loop() срабатывает снова и снова, в бесконечном цикле!
void loop() {
    digitalWrite(LED_BUILTIN, HIGH); // Вкл. светодиод (HIGH -- высокий ур. напряжения)
    delay(waitTime); // Ждём заданное число мс
    digitalWrite(LED_BUILTIN, LOW); // Выкл. светодиод (LOW -- низкий ур. напряжения)
    delay(waitTime); // Ждём заданное число мс
}
