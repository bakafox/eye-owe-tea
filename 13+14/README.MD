# Задание 13

Реализовать двустороннее взаимодействие Удаленного Терминала с его цифровым двойником в облачной платформе ThingWorx.

Решение данной задачи потребует создания шаблона вещи RemoteTerminalTemplate, а также самого цифрового двойника Удаленного Терминала - вещи RemoteTerminal.

Двустороннее враимодействие должно осуществляться посредством сервиса ввода-вывода InOut (либо InputOutput, если исполнителю так больше нравится).

Порядок настройки:
1. Thing — название вашей вещи (напр. RemoteTerminal)
2. Service — название сервиса этой вещи (напр. InOut)
3. AppKey — keyId вашего ключа приложения из ThingWorx
4. ServerIP — сочетание IP:ПОРТ (напр. 127.0.0.1:443)
5. Подключиться — если ошибка, перепроверить данные

Ссылки по теме:
- https://drive.google.com/file/d/1a_QlT9ALZPbWKRNeQeW4ihlrT393yae6/view?usp=classroom_web
- https://drive.google.com/file/d/1yDcA4VkircuvKYn3AZrx5OWjb043Orh5/view?usp=classroom_web


# Задание 14

Разработать мэшап, обеспечивающий мониторинг и изменение состояния Удаленного Терминала. Мэшап должен отображать текущее состояние всех параметров (как параметров для мониторинга, так и параметров для управления), а также обеспечивать изменение значений параметров для управления.

Продемонстрировать взаимодействие мэшапа и симулятора Удаленного Терминала.

Ссылки по теме:
- http://generalrobotics.ru/node/63
