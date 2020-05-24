# wxCAN Sniffer
CAN bus sniffer (PC side)

Используется wxWidgets 3.1.3.

====  Компиляция wxWidgets  ====

1. Скачать и установить wxWidgets если это установщик, либо распаковать, если это архив:

      https://www.wxwidgets.org/downloads/

2. Создать переменную окружения WXWIN указывающую на папку куда установили или распаковали (например C:\wxWidgets):

      Свойства системы -> Дополнительные параметры системы -> Переменные среды -> Создать
      WXWIN = C:\wxWidgets

3. Из папки C:\wxWidgets\build\msw открыть файл решения под соответствующую Visual Studio (wx_vc16.sln для Visual Studio 2019)

4. В Solution Expolorer, с помощью клавиши Shift, выделить все проекты, кроме _custom_build и зайти в Properties проектов.

5. В разделе C/C++ -> Code Generation изменить параметр Runtime Library:

      Для конфигурации Debug выбрать /MTd
      
      Для конфигурации Release выбрать /MT

6. Скомпилировать библиотеки wxWidgets по очереди для Debug и Release конфигураций.

====  Пробное приложение  ====

1. В Visual Studio создать Empty Project с указанием типа приложения Desktop Application (.exe)

2. В окне View -> Property Manager для своего проекта правой кнопкой выбрать меню Add existing property sheet... и выбрать файл:

      C:\wxWidgets\wxwidgets.props

3. Создать файл main.cpp и скопировать в него содержимое файла:

      C:\wxWidgets\samples\minimal\minimal.cpp

4. В настройках проекта C/C++ -> Code Generation изменить (если пункт не появился - сделать пробную сборку):

    Runtime Library для конфигурации Debug:  /MTd
    Runtime Library для конфигурации Release: /MT

5. Дополнительно, если необходимы привилегии UAC, в разделе Linker -> Manifest File:

    UAC Execution Level: requireAdministrator

6. Для добавления иконки exe-файлу надо добавить ресурсный файл со следующим содержимым:

    #include "wx\msw\wx.rc"
    wxicon icon app_icon.ico
