# CAN sniffer (приложение для компьютера)

[![Translate](https://img.shields.io/badge/Translate_to-ENGLISH-blue.svg?style=plastic)](https://github-com.translate.goog/KruFFT/wxCAN-Sniffer?_x_tr_sl=ru&_x_tr_tl=en)

[Статья о проекте](https://habr.com/ru/post/479672)

[Код для микроконтроллера](https://github.com/KruFFT/CAN-Sniffer)

---

- [Возможности](#Возможности)
- [Сборка приложения в Windows](#Сборка-приложения-в-Windows)
  - [Используя CMake](#Используя-CMake)
  - [Используя Visual Studio 2022 Community](#Используя-Visual-Studio-2022-Community)
    - [Сборка wxWidgets](#Сборка-wxWidgets)
    - [Сборка приложения в Visual Studio](#Сборка-приложения-в-Visual-Studio)
- [Сборка приложения в Linux](#Сборка-приложения-в-Linux)
  - [Используя CMake](#Используя-CMake)
  - [Используя Visual Studio Code](#Используя-Visual-Studio-Code)
    - [Сборка wxWidgets](#Сборка-wxWidgets)
    - [Сборка приложения в Visual Studio Code](#Сборка-приложения-в-Visual-Studio-Code)
- [Сборка приложения в macOS](#Сборка-приложения-в-macOS)
- [License](#License)

### Возможности
- Подключение через последовательный порт или Wi-Fi
- Отображение передаваемых в CAN-шине пакетов
- Отображение выбранных данных в двоичном и десятичном виде
- Построение графика по выбранному пакету + следующий байт/байты
- Сохранение всех данных в журнал
- Отправка своего пакета в CAN-шину

---

### Сборка приложения в Windows
#### Используя CMake
1. Понадобится [Visual Studio 2022 Community](https://visualstudio.microsoft.com/ru/downloads/) (используется компилятор и SDK) и [CMake](https://cmake.org/download/)
2. Запустить **Developer Command Prompt for VS 2022** и перейти в директорию с исходниками
3. Запустить подготовку проекта к сборке, будет автоматически скачан и подготовлен пакет **wxWidgets** в директорию `./build`:
```
cmake -S . -B build
```
4. Запустить сборку (результат будет в `./build/Release`):
```
cmake --build build -j --config Release
```
5. Если необходима отладочная сборка (результат будет в `./build/Debug`):
```
cmake --build build -j
```

Если по каким-либо причинам подготовка завершается с ошибкой, то в файле **CMakeLists.txt** можно указать версию wxWidgets убрав комментарий со строки **GIT_TAG v3.2.8** и задав в ней номер версии.

#### Используя Visual Studio 2022 Community
Сначала необходимо собрать **wxWidgets** в static-режиме и потом само приложение.

##### Сборка wxWidgets
1. Понадобится [Visual Studio 2022 Community](https://visualstudio.microsoft.com/ru/vs/community)
2. Запустить **Developer Command Prompt for VS 2022** и перейти в директорию с исходниками
3. Загрузить wxWidgets из GitHub:
```
git clone --recurse-submodules https://github.com/wxWidgets/wxWidgets.git
```
4. Создать переменную окружения `WXWIN` и присвоить ей значение директории `%your_path%/wxWidgets`
5. Запустить последовательно сборку отладочной и релизной версии:
```
cd wxWidgets/build/msw
nmake /f makefile.vc RUNTIME_LIBS=static TARGET_CPU=X64 BUILD=debug
nmake /f makefile.vc RUNTIME_LIBS=static TARGET_CPU=X64 BUILD=release
```

##### Сборка приложения в Visual Studio
1. Открыть файл решения `wxCAN-Sniffer.sln`
2. Выбрать необходимую конфигурацию **Release** или **Debug**
3. Произвести сборку нажав **F7** (результат будет в `./x64/Release` или `./x64/Debug` соответственно)

---

### Сборка приложения в Linux
#### Используя CMake
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
или отладочной сборки в `./build-debug`:
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

PS: По каким-то причинам, сборка через CMake в Linux требует огромных ресурсов памяти. На компьютере с 8 ГБ оперативной памяти сборка не завершалась. Помогло увеличение размера swap-файла до 64 ГБ:
```
sudo swapoff /swapfile
sudo fallocate -l 64G /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
```

Если по каким-либо причинам подготовка завершается с ошибкой, то в файле **CMakeLists.txt** можно указать версию wxWidgets убрав комментарий со строки **GIT_TAG v3.2.8** и задав в ней номер версии.

#### Используя Visual Studio Code
##### Сборка wxWidgets
1. Понадобится [Visual Studio Code](https://code.visualstudio.com/download/) и установить следующие пакеты:
```
sudo apt-get install build-essential
sudo apt-get install libgtk-3-dev
sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev
```
2. Скачать и установить wxWidgets:
```
git clone --recurse-submodules https://github.com/wxWidgets/wxWidgets.git
```
3. Подготовить директорию `./gtk-build` для сборки и запустить настройку:
```
mkdir gtk-build
cd gtk-build
../configure --with-gtk=3 --with-opengl --disable-shared
```
4. Запустить сборку (использовать 8 потоков):
```
make -j8
```
5. Установить собранный пакет в систему:
```
sudo make install
sudo ldconfig
```
6. Скопировать файл **setup.h** (обратите внимание на номер версии в директориях, если версия отличается от 3.3, то её надо поправить):
```
sudo cp ./lib/wx/include/gtk3-unicode-static-3.3/wx/setup.h /usr/local/include/wx-3.3/wx
```

##### Сборка приложения в Visual Studio Code
1. Открыть директорию проекта `./wxCAN-Sniffer/wxCAN-Sniffer` в редакторе Visual Studio Code
2. Открыть файл **Application.cpp**
3. В меню **Terminal** выбрать пункт **Run Task**, выбрать необходимую конфигурацию **C/C++: GCC build release** или **C/C++: GCC build debug** (результат будет в этой же директории)

---

### Сборка приложения в macOS
Сборка и работа приложения возможна.

---

### License
MIT
