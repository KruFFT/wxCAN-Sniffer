#pragma once

#include <wx/wx.h>
#include <wx/ffile.h>
#include "Parameters.h"

#ifdef __WINDOWS__
    #define MEMCOPY(dest, source, size) memcpy_s(dest, size, source, size)
#endif
#ifdef __LINUX__
    #include <stdint.h>
    #define MEMCOPY(dest, source, size) memcpy(dest, source, size);
#endif
#ifdef __APPLE__
    #include <stdint.h>
    #define MEMCOPY(dest, source, size) memcpy(dest, source, size);
#endif

#define VENDOR_NAME                 wxT("KruFFT")
#define APPLICATION_NAME            wxT("wxCAN-Sniffer")
#define SETTINGS_FILE_NAME          wxT("wxCAN-Sniffer.ini")
#define CAPTION                     wxT("CAN Sniffer 2.4.0")

#define UDP_BUFFER_SIZE             1000        // размер сетевого буфера приёма пакетов

#define FORMAT_HEX2                 wxT("%02X")
#define FORMAT_HEX3                 wxT("%03X")
#define FORMAT_HEX8                 wxT("%08X")
#define FORMAT_INT                  wxT("%i")
#define FORMAT_UINT                 wxT("%u")
#define FORMAT_INT0                 wxT("%0i")
#define FORMAT_INT3                 wxT("%03i")
#define FORMAT_FLOAT1_0             wxT("%1.0f")
#define FORMAT_FLOAT1_3             wxT("%1.3f")

#define TEXT_CAN_PACKETS_VIEW       wxT("Просмотр пакетов")
#define TEXT_CAN_ID                 wxT("CAN ID")
#define TEXT_CAN_INTERVAL           wxT("Интервал")
#define TEXT_CAN_LENGTH             wxT("Длина")
#define TEXT_CAN_BYTE_0             wxT("Байт 0")
#define TEXT_CAN_BYTE_1             wxT("Байт 1")
#define TEXT_CAN_BYTE_2             wxT("Байт 2")
#define TEXT_CAN_BYTE_3             wxT("Байт 3")
#define TEXT_CAN_BYTE_4             wxT("Байт 4")
#define TEXT_CAN_BYTE_5             wxT("Байт 5")
#define TEXT_CAN_BYTE_6             wxT("Байт 6")
#define TEXT_CAN_BYTE_7             wxT("Байт 7")
#define TEXT_CAN_TEST               wxT("Тестовая отправка пакета и просмотр ответа")
#define TEXT_CAN_TEST_PACKET        wxT("CAN пакет:")
#define TEXT_CAN_TEST_ID            wxT("7E0")
#define TEXT_CAN_TEST_LENGTH        wxT("8")
#define TEXT_CAN_TEST_BYTE_0        wxT("05")
#define TEXT_CAN_TEST_BYTE_1        wxT("2F")
#define TEXT_CAN_TEST_BYTE_2        wxT("00")
#define TEXT_CAN_TEST_BYTE_3        wxT("0A")
#define TEXT_CAN_TEST_BYTE_4        wxT("06")
#define TEXT_CAN_TEST_BYTE_5        wxT("FF")
#define TEXT_CAN_TEST_BYTE_6        wxT("00")
#define TEXT_CAN_TEST_BYTE_7        wxT("00")
#define TEXT_CAN_TEST_ANSWER        wxT("Отображать ответ от:")
#define TEXT_CAN_TEST_ANSWER_ID     wxT("7E8")
#define TEXT_CAN_TEST_SEND          wxT("Отправить CAN-пакет")
#define TEXT_CAN_TEST_CLEAR         wxT("Очистить список ответов")
#define TEXT_MANAGEMENT             wxT("Управление")
#define TEXT_PORT                   wxT("Порт")
#define TEXT_NETWORK                wxT("Сеть")
#define TEXT_CONNECTION_MODE        wxT("Режим подключения")
#define TEXT_CONNECT                wxT("Подключить")
#define TEXT_DISCONNECT             wxT("Отключить")
#define TEXT_CR_SPACES              wxT("\n  ")
#define TEXT_CR_CR                  wxT("\n\n")
#define TEXT_SERIAL_PORT            wxT("Последовательный порт")
#define TEXT_MC_IP_ADDRESS          wxT("IP-адрес микроконтроллера")
#define TEXT_CAN_SPEED_100          wxT("100")
#define TEXT_CAN_SPEED_125          wxT("125")
#define TEXT_CAN_SPEED_200          wxT("200")
#define TEXT_CAN_SPEED_250          wxT("250")
#define TEXT_CAN_SPEED_500          wxT("500")
#define TEXT_CAN_SPEED_1000         wxT("1000")
#define TEXT_CAN_SPEED              wxT("Скорость CAN-шины")
#define TEXT_PPS                    wxT("пакетов/с")
#define TEXT_BPS                    wxT("байтов/с (исключая служебную информацию)")
#define TEXT_LOG_WRITE              wxT("Запись в журнал")
#define TEXT_LOG_WRITE_ENABLE       wxT("Вести запись в журнал")
#define TEXT_LOG_ADD_ID             wxT("Добавить ID в фильтр >>")
#define TEXT_LOG_REMOVE_ID          wxT("Убрать ID из фильтра <<")
#define TEXT_LOG_CLEAR_IDS          wxT("Очистить фильтр")
#define TEXT_LOG_FILE_TYPE          wxT("Тип файла:")
#define TEXT_LOG_FILE_EXT_CSV       wxT(".csv")
#define TEXT_LOG_FILE_EXT_LOG       wxT(".log")
#define TEXT_LOG_SEPARATOR          wxT("Разделитель:")
#define TEXT_LOG_SEPARATOR_SEMICOLON wxT(";")
#define TEXT_LOG_SEPARATOR_TAB      wxT("Tab")
#define TEXT_LOG_SEPARATOR_MINUS    wxT("-")
#define TEXT_LOG_SEPARATOR_UNDERSCORE wxT("_")
#define TEXT_LOG_SEPARATOR_SPACE    wxT("Space")
#define TEXT_LOG_SEPARATOR_TAB_VALUE wxT("\t")
#define TEXT_LOG_SEPARATOR_SPACE_VALUE wxT(" ")
#define TEXT_LOG_SAVE_TO_ONE_FILE   wxT("Сохранение данных в один файл")
#define TEXT_LOG_DECIMAL            wxT("Десятичный вывод данных")
#define TEXT_LOG_ADD_ASCII          wxT("Добавлять ASCII данные")
#define TEXT_LOG_FILE_PREFIX        wxT("CAN")
#define TEXT_LOG_FILE_PREFIX_ID     wxT("/CAN ID ")
#define TEXT_LOG_FILE_ACCESS        wxT("a")
#define TEXT_LOG_FILE_ASCII_SEPARATOR wxT('|')
#define TEXT_DECODING               wxT("Декодированные значения")
#define TEXT_DECODING_DATA_TYPE     wxT("Тип данных:")
#define TEXT_DECODING_UINT8         wxT("UInt8")
#define TEXT_DECODING_UINT16        wxT("UInt16")
#define TEXT_DECODING_UINT32        wxT("UInt32")
#define TEXT_DECODING_INT8          wxT("Int8")
#define TEXT_DECODING_INT16         wxT("Int16")
#define TEXT_DECODING_INT32         wxT("Int32")
#define TEXT_DECODING_FLOAT         wxT("Float")
#define TEXT_DECODING_BE            wxT("Big-endian")
#define TEXT_DECODING_BINARY        wxT("Двоичный вид:")
#define TEXT_DECODING_DECIMAL       wxT("Десятичный вид:")
#define TEXT_DECODING_MULTIPLIER    wxT("Множитель:")
#define TEXT_DECODING_RESULT        wxT("Результат:")
#define TEXT_DECIMAL_SEPARATOR      wxT(".")
#define TEXT_SPACE                  wxT(" ")
#define TEXT_0                      wxT('0')
#define TEXT_1                      wxT('1')
#define TEXT_NUMBER_BLOCK_SEPARATOR wxT('_')

#define ERROR_CAPTION               wxT("Ошибка")
#define ERROR_SERIAL                wxT("Невозможно работать с этим последовательным портом")
#define ERROR_SERIAL_OPEN           wxT("Невозможно открыть порт.\nОшибка: 0x")
#define ERROR_SERIAL_SET_PARAMETERS wxT("Невозможно установить параметры порта.\nОшибка: 0x")
#define ERROR_SERIAL_READ           wxT("Ошибка чтения данных: 0x")
#define ERROR_UDP_OPEN              wxT("Ошибка открытия UDP-сокета: ")

#define ERROR_THREAD_CREATE         "Невозможно создать поток"
#define ERROR_THREAD_START          "Невозможно запустить поток"

#define SWAP_BYTES_UINT16(value)    ((((value) >> 8) & 0x00FF) | (((value) << 8) & 0xFF00))
#define SWAP_BYTES_UINT24(value)    ((((value) >> 16) & 0x000000FF) | (value & 0x0000FF00) | (((value) << 16) & 0x00FF0000))
#define SWAP_BYTES_UINT32(value)    ((((value) >> 24) & 0x000000FF) | (((value) >> 8) & 0x0000FF00) | (((value) << 8) & 0x00FF0000) | (((value) << 24) & 0xFF000000))
#define SWAP_BYTES_UINT64(value)    ((((value) >> 56) & 0x00000000000000FF) | (((value) >> 40) & 0x000000000000FF00) | (((value) >> 24) & 0x0000000000FF0000) | (((value) >>  8) & 0x00000000FF000000) | (((value) <<  8) & 0x000000FF00000000) | (((value) << 24) & 0x0000FF0000000000) | (((value) << 40) & 0x00FF000000000000) | (((value) << 56) & 0xFF00000000000000))

// CAN-пакет для приёма данных
#pragma pack (push, 1)
struct CANFrameIn
{
public:
    uint32_t id;            // идентификатор пакета
    uint16_t interval;      // интервал между пакетами (мс)
    uint8_t  length;        // длина пакета
    uint8_t  data[8];       // массив данных пакета, до 8 байт
};
#pragma pack(pop)

#define NEW_PACKET      0   // новый пакет
#define UPDATED_PACKET  1   // обновлённый пакет
#define TIMEOUT_PACKET  100 // не более 255! давно не обновлявшийся пакет (выцвел) - косвенно задаёт время выцветания пакета тактами по 10 мс
#define PACKET_DELTA    TIMEOUT_PACKET - UPDATED_PACKET // шаг выцветания пакета

// CAN-пакет для отображения в таблице
#pragma pack (push, 1)
struct VisualCANFrame
{
public:
    CANFrameIn frame;       // пакет с данными
    wxColour   color[8];    // цвет фона каждой ячейки
    uint8_t    ticks[8];    // счётчик тактов обновления цвета: 101 - новый пакет, 100 - обновлённый пакет, 0 - пакет "выцвел"

    // оператор сравнения CAN-пакета для сортировки
    bool operator < (const VisualCANFrame& anotherFrame) const
    {
        return (frame.id < anotherFrame.frame.id);
    }
};
#pragma pack(pop)

// CAN-пакет для отправки данных
#pragma pack (push, 1)
struct CANFrameOut
{
public:
    uint32_t id;            // идентификатор пакета
    uint16_t reserved;      // зарезервировано (скорее всего для интервальной отправки)
    uint8_t  length;        // длина пакета
    uint8_t  data[8];       // массив данных пакета, до 8 байт
};
#pragma pack(pop)

#pragma pack (push, 1)
struct SendCANFrame
{
public:
    uint32_t Signature;
    CANFrameOut Frame;      // пакет для отправки
};
#pragma pack(pop)

// Описание log-файла
struct LogFile
{
public:
    uint32_t id;            // идентификатор пакета
    wxFFile* file;          // ассоциированный файл
};

// Режимы работы
enum Modes
{
    Serial = 0,
    Network
};

// Команды управления
enum CANCommands
{
    Disconnect = 0,
    Connect
};

/*

* Пакет статистики от микроконтроллера к компьютеру:
    От микроконтроллера приходит пакет с сервисным ID длиной 4 байта со статистикой пакетов.

    +-----------+----------+-------+--------------+--------------+--------------+--------------+
    | ID        | Интервал | Длина |    Байт 0    |    Байт 1    |    Байт 2    |    Байт 3    |
    +-----------+----------+-------+--------------+--------------+--------------+--------------+
    | ServiceID |    --    |   4   |             FPS             |             BPS             |
    |           |          |       | Младший байт | Старший байт | Младший байт | Старший байт |
    +-----------+----------+-------+--------------+--------------+--------------+--------------+

    - FPS - frames per second, счётчик принятых из CAN-шины кадров в секунду.
    - BPS - bytes per second, счётчик принятых из CAN-шины байтов полезной информации,
            сумма длины идентификатора пакета (4 байта) + байта длины данных + количество
            принятых байтов данных.

* Протокол управления от компьютера к микроконтроллеру:
    Формируется пакет с сервисным ID, заполняется команда с параметрами и отправляется
    в микроконтроллер. Вместе с сигнатурой получается:
    4 + 4 + 2 + 1 + 8 = 19 байт

    +-----------+----------+-------+---------+--------+--------+--------+--------+--------+--------+--------+
    | ID        | Интервал | Длина |  Байт 0 | Байт 1 | Байт 2 | Байт 3 | Байт 4 | Байт 5 | Байт 6 | Байт 7 |
    +-----------+----------+-------+---------+--------+--------+--------+--------+--------+--------+--------+
    | ServiceID |    --    |   x   | Команда |                          Параметры                           |
    +-----------+----------+-------+---------+--------+--------+--------+--------+--------+--------+--------+

    Команды:
    - 0 - Отключиться от CAN-шины, без параметров.

    +-----------+----------+-------+---------+--------+--------+--------+--------+--------+--------+--------+
    | ID        | Интервал | Длина |  Байт 0 | Байт 1 | Байт 2 | Байт 3 | Байт 4 | Байт 5 | Байт 6 | Байт 7 |
    +-----------+----------+-------+---------+--------+--------+--------+--------+--------+--------+--------+
    | ServiceID |    --    |   1   | Команда |                              --                              |
    |           |          |       |    0    |                                                              |
    +-----------+----------+-------+---------+--------+--------+--------+--------+--------+--------+--------+

    - 1 - Подключиться на заданной скорости, инициализирует CAN-модуль для подключения на указанной
          скорости в кбит/с(значение передаётся в двух байтах параметров). При использовании 16 бит
          это позволит задать скорости от 1 кбит/с до 65536 кбит/с.

    +-----------+----------+-------+---------+--------------+--------------+--------+--------+--------+--------+--------+
    | ID        | Интервал | Длина |  Байт 0 |    Байт 1    |    Байт 2    | Байт 3 | Байт 4 | Байт 5 | Байт 6 | Байт 7 |
    +-----------+----------+-------+---------+--------------+--------------+--------+--------+--------+--------+--------+
    | ServiceID |    --    |   3   | Команда |     Скорость CAN-шины       |                    --                      |
    |           |          |       |    1    | Младший байт | Старший байт |                                            |
    +-----------+----------+-------+---------+--------------+--------------+--------+--------+--------+--------+--------+

*/
