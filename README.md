# wxCAN Sniffer
## _CAN bus sniffer (PC side)_ ##

#### Возможности
- Отображение передаваемых в CAN-шине пакетов
- Сохранение всех данных в журнал
- Отображение выбранных данных в двоичном и десятичном виде
- Построение графика по выбранному пакету + следующий байт
- Отправка своего пакета в CAN-шину

## Статья
[О проекте](https://habr.com/ru/post/479672)

## Сборка приложения используя CMake
1. Понадобится [Visual Studio 2022 Community](https://visualstudio.microsoft.com/ru/downloads/) (используется компилятор и SDK)
2. Распаковать исходники
3. Запустить **Developer Command Prompt for VS 2022** и перейти в директорию с исходниками
4. Запустить подготовку проекта к сборке, будет автоматически скачан и подготовлен пакет **wxWidgets** в директорию `./build`:
```sh
cmake -S . -B build
```
5. Запустить сборку (результат будет в `./build/Release`):
```sh
cmake --build build -j --config Release
```
6. Если необходима отладочная сборка (результат будет в `./build/Debug`):
```sh
cmake --build build -j
```

## Сборка приложения используя Visual Studio 2022
Сначала необходимо собрать **wxWidgets** в static-режиме и потом само приложение.

#### Сборка wxWidgets
1. Понадобится [Visual Studio 2022 Community](https://visualstudio.microsoft.com/ru/downloads/)
2. Скачать и установить [wxWidgets](https://www.wxwidgets.org/downloads/) если это установщик, либо распаковать, если это архив. Например в директорию `C:/wxWidget`
3. Создать переменную окружения `WXWIN` и присвоить ей значение директории `C:/wxWidget`
4. В директории `C:/wxWidgets/build/msw` открыть файл решения `wx_vc17.sln` для Visual Studio 2022)
5. В **Solution Explorer**, с помощью клавиши Shift, выделить все проекты, кроме **_custom_build** и зайти в **Properties** проектов.
6. В разделе **C/C++** → **Code Generation** изменить параметр **Runtime Library**:
    - для конфигурации **Debug** выбрать **/MTd**
    - для конфигурации **Release** выбрать **/MT**
7. Нажать **Ok** и скомпилировать (**Ctrl+B**) библиотеки wxWidgets по очереди для Debug и Release конфигураций.
#### Сборка приложения в Visual Studio
1. Распаковать исходники
2. Открыть файл решения `wxCAN-Sniffer.sln`
3. Выбрать необходимую конфигурацию **Debug** или **Release**
4. Произвести сборку **F7** (результат будет в `./x64/Debug` и `./x64/Release` соответственно)
    
## License
MIT