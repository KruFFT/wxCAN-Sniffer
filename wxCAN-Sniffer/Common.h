#pragma once

//#include <wx/wxprec.h>
//#ifndef WX_PRECOMP
	#include <wx/wx.h>
//#endif
#include <wx/grid.h>
#include <wx/splitter.h>
#include <wx/ffile.h>
#include <wx/thread.h>
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <wx/socket.h>
#include <windows.h>
#include <stdint.h>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

#define CAPTION		wxT("CAN Sniffer 2.1.1")

#define SIG_BYTE_0	0xAA
#define SIG_BYTE_1	0x55
#define SIG_BYTE_2	0xAA
#define SIG_BYTE_3	0x55
#define SIG_DWORD	(uint32_t)(SIG_BYTE_3 << 24 | SIG_BYTE_2 << 16 | SIG_BYTE_1 << 8 | SIG_BYTE_0)

#define UDP_PORT			0xAA55	// UDP порт
#define UDP_BUFFER_SIZE		1000	// размер буфера приёма пакетов
#define CAN_DATA_MINIMAL	19		// минимальный размер данных для парсера: сигнатура (4 байта) + ID пакета (4 байта) + интервал (2 байта) + длина (1 байт) + данные (8 байт) = 19 байт

// CAN-пакет для приёма данных
#pragma pack (push, 1)
struct CANFrameIn
{
public:
	uint32_t id;			// идентификатор пакета
	uint16_t interval;		// интервал между пакетами (мс)
	uint8_t  length;		// длина пакета
	uint8_t  data[8];		// массив данных пакета, до 8 байт
};
#pragma pack(pop)

// CAN-пакет для отображения в таблице
#pragma pack (push, 1)
struct VisualCANFrame
{
public:
	CANFrameIn frame;		// пакет с данными
	wxColour color[8];		// цвет фона ячейки

	// оператор сравнения CAN-пакета необходим для сортировки
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
	uint32_t id;			// идентификатор пакета
	uint8_t  length;		// длина пакета
	uint8_t  data[8];		// массив данных пакета, до 8 байт
};
#pragma pack(pop)

#pragma pack (push, 1)
struct SendCANFrame
{
public:
	uint32_t Signature = SIG_DWORD;
	CANFrameOut Frame;		// пакет для отправки
};
#pragma pack(pop)

// Описание log-файла
struct LogFile
{
public:
	uint32_t id;			// идентификатор пакета
	wxFFile* file;			// хэндл ассоциированного файла
};

#define CONNECT						wxT("Подключить")
#define DISCONNECT					wxT("Отключить")

#define FORMAT_HEX2					wxT("%02X")
#define FORMAT_HEX3					wxT("%03X")
#define FORMAT_HEX8					wxT("%08X")
#define FORMAT_INT					wxT("%i")
#define FORMAT_INT0					wxT("%0i")
#define FORMAT_INT3					wxT("%03i")
#define FORMAT_FLOAT				wxT("%1.6f")

#define ERROR_CAPTION				wxT("Ошибка")
#define ERROR_SERIAL_GET_PARAMETERS	wxT("Невозможно получить параметры порта.\nОшибка: ")
#define ERROR_SERIAL_SET_PARAMETERS	wxT("Невозможно установить параметры порта.\nОшибка: ")
#define ERROR_SERIAL_SET_TIMEOUTS	wxT("Невозможно установить тайм-ауты порта.\nОшибка: ")
#define ERROR_SERIAL				wxT("Невозможно работать с этим последовательным портом")
#define ERROR_UDP_OPEN				wxT("Ошибка открытия UDP-сокета: "

#define ERROR_THREAD_CREATE			"Невозможно создать поток"
#define ERROR_THREAD_START			"Невозможно запустить поток"
