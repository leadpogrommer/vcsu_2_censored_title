# VCSU 2: Electric Boogaloo

GUI Js программы на esp32

## Структура репозитория
- `labXX` - прошивка для esp32
- `cnc` (Command and Control) - бэкенд
- `cnc_ui` - фронтенд

## Архитектура
### Прошивка
Прошивка написана под esp-idf. Для отрисовки элементов интерфейса используется `lvgl`, для многозадачности - `FreeRTOS`, для исполнения джаваскрипта - `duktape`.
При запуске программы создаётся новая FreeRTOS таска и новый экземпляр duktape. 

Duktape позволяет достаточно легко добавлять новые объекты в глобальную область видимости, все такие объекты описаны в [esp_lvgl_builtins.yaml](https://github.com/leadpogrommer/duktape/blob/leadpogrommers_shit/esp_lvgl_builtins.yaml) в моём форке duktape'а.

Код JS приложений событийно-ориентированный, при запуске их код создаёт элементы интерфейса, устанавливает колбэки и таймауты и завершается, после чего запускается event-loop, в котором исполняются ранее установленные колбэки при появлении событий.



Разбиение (кривое) по "модулям":
- [main.cpp](labXX/main/main.cpp) - main, инициализирует все
- [display.c](labXX/main/display.c) - инициализирует lvgl и i2c дисплей
- [network.c](labXX/main/network.c) - управляет wifi и websocket соединениями, пытается их переподключать, если отваливаются, обрабатывает rpc запросы от бэкенда (`handle_rpc_data`)
- [input.cpp](labXX/main/input.cpp) - обрабатывает события с энуодера (использует third party компонент) и кнопки
- [telemetry.cpp](labXX/main/telemetry.cpp) - отсылает на бэк скриншот, список задач и объём свободной памяти в куче раз в 500 мс
- [gui.cpp](labXX/main/gui.cpp) - Создание/удаление JS тасок, а так же JS event loop
- [taskmgr.cpp](labXX/main/taskmgr.cpp) - Высокоуровневое (относительно `gui.cpp`) управление тасками. Хранит список запущенных тасок, млдержит логику для добавления, удаления и переключения тасок, передачи ввода активной таске.
- [bindings_gui.cpp](labXX/main/bindings_gui.cpp) - JS биндинги для к частям LVGL
- [bindings_timeout.cpp](labXX/main/bindings_timeout.cpp) - реализация `setTimeout` и `print`
- [bindings_io.cpp](labXX/main/bindings_io.cpp) - реализация `digitalRead` и `digitalWrite`

### Бэкенд
Написан на питоне, интересного функционала не содержит.
Пересылает сообщения от устройств всем подключенным фронтенд-клиентам, сообщения от фронтенд-клиентов - конкретным устройствам. Следит за добавлением/удалением программ, уведомляет об этом устройства и фронтенд. Отправляет на устройство код на исполнение при запросе с устройства или с клиента (**ограничение на размер программы - 3кб**).

### Фронтенд
Написан на flutter. 

## JS API
Примеры программ [тут](cnc/programs).
### `LVGLObject`
Базовый класс для всех виджетов. Содержит сеттеры `x`, `y`, `w` и `h`, устанавливающие соответствующие свойства LVGL объектов. Не имеет конструктора.
### `LVGLLabel(): LVGLObject`
Отображает текст. Текст можно установить через сеттер `text`
### `LVGLButton(cb): LVGLObject`
Вызывает функцию `cb` при нажатии
### `LVGLSpinbox(cb, min, max, step, decimal_places)`
Вызывает функцию `cb` при изменении значения, передаёт новое значение единственным аргументом.
### `print(...args)`
Печатает аргументы в лог через пробел
### `setTimeout(cb, delay)`
Создаёт таймер, по истечении которого вызывается `cb`. Не блокирует поток.
### `pinMode(pin, 0)`
Инициализирует `pin` для использования `digitalWrite`
### `digitalWrite(pin, bool_value)`
Устанавливает логический уровень на пине `pin`

## Сборка
Склонировать репозиторий **рекурсивно**

### Фронтенд
`flutter build linux`

### Бэкенд
`poetry install` (нужен python 3.12 и poetry)

Для запуска: 
- `poetry shell`
- `python cnc/main.py`

### Прошивка
Нужны:
- esp-idf v3.5.1
- pyenv

Сборка:
- создать в папке main файл `wifi_creds.h`, в которм объявит макросы
    - `WIFI_SSID`
    - `WIFI_PASSWORD`
    - `WIFI_PASSWORD` (в формате ip:port)
- `pyenv install 2.7.18`
- `cd thirdparty/duktape && PATH="$(pyenv prefix 2.7.18)/bin:$PATH" make prep/duk-esp-debug && cd ../..`
- `source <путь к esp-idf>/export.sh`
- `idf.py build`
