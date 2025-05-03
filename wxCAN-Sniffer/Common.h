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

#define CONNECT                     wxT("Подключить")
#define DISCONNECT                  wxT("Отключить")

#define FORMAT_HEX2                 wxT("%02X")
#define FORMAT_HEX3                 wxT("%03X")
#define FORMAT_HEX8                 wxT("%08X")
#define FORMAT_INT                  wxT("%i")
#define FORMAT_UINT                 wxT("%u")
#define FORMAT_INT0                 wxT("%0i")
#define FORMAT_INT3                 wxT("%03i")
#define FORMAT_FLOAT1_0             wxT("%1.0f")
#define FORMAT_FLOAT1_3             wxT("%1.3f")

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
