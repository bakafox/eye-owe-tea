# Задание 12

Познакомиться с Composer, выполнив создание следующих сущностей:
1. ThingShape.
2. ThingTemplate.
3. Thing.

В качестве объекта моделирования предлагается использовать комбинированный датчик температуры-влажности-давления.

Реализация комбинированного датчика температуры-влажности-давления потребует реализацию трех форм вещей, обладающих соответствующими свойствами и реализающих соответствующие getter- и setter-сервисы:

1. TemperatureSensorShape:
    - Property: temperature.
    - Services: SetTemperature, GetTemperature.
2. HumiditySensorShape:
    - Property: humidity.
    - Services: SetHumidity, GetHumidity.
3. PressureSensorShape:
    - Property: pressure.
    - Services: SetPressure, GetPressure.

Также потребуется создание шаблона вещи CombinedSensorTemplate и вещи CombinedSensor.

Работоспособность функционала CombinedSensor предлагается продемонстрировать посредством выполнения внешнеи вызовов getter- и setter-сервисов посредством cURL, Postman, либо другим удобным способом, например, посредством вызовов из скриптов на Python.
