# wxCAN-Sniffer - CAN bus sniffer (PC side)

[TOC]
- [Возможности](# Возможности)
- [Статья](# Статья)
- [Сборка приложения в Windows](# Сборка приложения в Windows)
  - [Используя Windows CMake](# Используя Windows CMake)
  - [Используя Visual Studio 2022 Community](# Используя Visual Studio 2022 Community)
    - [1. Сборка Windows wxWidgets](# 1. Сборка Windows wxWidgets)
    - [2. Сборка приложения в Visual Studio](# 2. Сборка приложения в Visual Studio)
- [Сборка приложения в Linux](# Сборка приложения в Linux)
  - [Используя Linux CMake](# Используя Linux CMake)
  - [Используя Visual Studio Code](# Используя Visual Studio Code)
    - [1. Сборка Linux wxWidgets](# 1. Сборка Linux wxWidgets)
    - [2. Сборка приложения в Visual Studio Code](# 2. Сборка приложения в Visual Studio Code)
- [License](# License)

## Возможности
- Отображение передаваемых в CAN-шине пакетов
- Сохранение всех данных в журнал
- Отображение выбранных данных в двоичном и десятичном виде
- Построение графика по выбранному пакету + следующий байт
- Отправка своего пакета в CAN-шину

## Статья
[О проекте](https://habr.com/ru/post/479672)

## Сборка приложения в Windows
### Используя Windows CMake
1. Понадобится [Visual Studio 2022 Community](https://visualstudio.microsoft.com/ru/downloads/) (используется компилятор и SDK) и [CMake](https://cmake.org/download/)
2. Запустить **Developer Command Prompt for VS 2022** и перейти в директорию с исходниками
3. Запустить подготовку проекта к сборке, будет автоматически скачан и подготовлен пакет **wxWidgets** в директорию `./build`:
```sh
cmake -S . -B build
```
4. Запустить сборку (результат будет в `./build/Release`):
```sh
cmake --build build -j --config Release
```
5. Если необходима отладочная сборка (результат будет в `./build/Debug`):
```sh
cmake --build build -j
```

Если по каким-либо причинам подготовка завершается с ошибкой, то в файле **CMakeLists.txt** можно указать версию wxWidgets раскомментировав строку **GIT_TAG v3.2.5** и задав в ней номер версии.

### Используя Visual Studio 2022 Community
Сначала необходимо собрать **wxWidgets** в static-режиме и потом само приложение.

#### 1. Сборка Windows wxWidgets
1. Понадобится [Visual Studio 2022 Community](https://visualstudio.microsoft.com/ru/downloads/)
2. Скачать и установить [wxWidgets](https://www.wxwidgets.org/downloads/) если это установщик, либо распаковать, если это архив. Например в директорию `C:/wxWidget`
3. Создать переменную окружения `WXWIN` и присвоить ей значение директории `C:/wxWidget`
4. В директории `C:/wxWidgets/build/msw` открыть файл решения `wx_vc17.sln` для Visual Studio 2022
5. В **Solution Explorer**, с помощью клавиши Shift, выделить все проекты, кроме **_custom_build** и зайти в **Properties** проектов
6. В разделе **C/C++** → **Code Generation** изменить параметр **Runtime Library**:
    - для конфигурации **Debug** выбрать **/MTd**
    - для конфигурации **Release** выбрать **/MT**
7. Нажать **Ok** и скомпилировать (**Ctrl+B**) библиотеки wxWidgets по очереди для Debug и Release конфигураций.

#### 2. Сборка приложения в Visual Studio
1. Открыть файл решения `wxCAN-Sniffer.sln`
2. Выбрать необходимую конфигурацию **Release** или **Debug**
3. Произвести сборку нажав **F7** (результат будет в `./x64/Release` или `./x64/Debug` соответственно)

## Сборка приложения в Linux
### Используя Linux CMake
1. Понадобится установить следующие пакеты:
```
sudo apt-get install build-essential
sudo apt-get install libgtk-3-dev
sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev
sudo apt-get install cmake
```
2. Запустить подготовку проекта к сборке, будет автоматически скачан и подготовлен пакет **wxWidgets** в директорию `./build-release`:
```sh
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
```
или отладчной сборки в `./build-debug`:
```sh
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
```
3. Запустить сборку Release (результат будет в `./build-release`):
```sh
cmake --build build-release -j
```
или Debug (результат будет в `./build-debug`):
```sh
cmake --build build-debug -j
```

По каким-то причинам, сборка через CMake в Linux требует огромных ресурсов памяти. На компьютере с 8 ГБ оперативной памяти сборка не завершалась. Помогло увеличение размера swap-файла до 64 ГБ:
```
sudo swapoff /swapfile
sudo fallocate -l 64G /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
```

Если по каким-либо причинам подготовка завершается с ошибкой, то в файле **CMakeLists.txt** можно указать версию wxWidgets раскомментировав строку **GIT_TAG v3.2.5** и задав в ней номер версии.

### Используя Visual Studio Code
#### 1. Сборка Linux wxWidgets
1. Понадобится [Visual Studio Code](https://code.visualstudio.com/download/) и установить следующие пакеты:
```
sudo apt-get install build-essential
sudo apt-get install libgtk-3-dev
sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev
```
2. Скачать и установить wxWidgets:
```
git clone https://github.com/wxWidgets/wxWidgets.git
```
3. Обновить необходимые зависимости:
```
cd wxWidgets
git submodule update --init src/stc/scintilla
git submodule update --init src/stc/lexilla
git submodule update --init 3rdparty/catch
git submodule update --init 3rdparty/nanosvg
```
4. Подготовить директорию `./gtk-build` для сборки и запустить настройку:
```
mkdir gtk-build
cd gtk-build
../configure --with-gtk=3 --with-opengl --disable-shared
```
5. Запустить сборку (использовать 8 потоков):
```
make -j8
```
6. Установить собранный пакет в систему:
```
sudo make install
sudo ldconfig
```
7. Скопировать файл **setup.h** (обратите внимание на номер версии в директориях, если версия отличается от 3.3, то их надо поправить):
```
sudo cp ./lib/wx/include/gtk3-unicode-static-3.3/wx/setup.h /usr/local/include/wx-3.3/wx
```

#### 2. Сборка приложения в Visual Studio Code
1. Открыть директорию проекта `./wxCAN-Sniffer/wxCAN-Sniffer` в редакторе Visual Studio Code
2. Открыть файл **Application.cpp**
3. В меню **Terminal** выбрать пункт **Run Task**, выбрать необходимую конфигурацию **C/C++: GCC build release** или **C/C++: GCC build debug** (результат будет в этой же директории)

## License
MIT