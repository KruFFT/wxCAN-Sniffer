#pragma once

#include <wx/wx.h>
#include <wx/ffile.h>

#ifdef __WINDOWS__
	#define MEMCOPY(dest, source, size)	memcpy_s(dest, size, source, size)
#endif
#ifdef __LINUX__
	#include <stdint.h>
	#define MEMCOPY(dest, source, size)	memcpy(dest, source, size);
#endif
#ifdef __APPLE__
	#include <stdint.h>
	#define MEMCOPY(dest, source, size)	memcpy(dest, source, size);
#endif

#define CAPTION				wxT("CAN Sniffer 2.2.0")

#define SIGNATURE_DWORD		0x55AA55AA	// сигнатура пакета (big-endian)
#define UDP_PORT			0xAA55		// UDP порт
#define UDP_BUFFER_SIZE		1000		// размер буфера приёма пакетов
#define CAN_DATA_MINIMAL	19			// минимальный размер данных для парсера: сигнатура (4 байта) + ID пакета (4 байта) + интервал (2 байта) + длина (1 байт) + данные (8 байт) = 19 байт

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
	uint32_t Signature = SIGNATURE_DWORD;
	CANFrameOut Frame;		// пакет для отправки
};
#pragma pack(pop)

// Описание log-файла
struct LogFile
{
public:
	uint32_t id;			// идентификатор пакета
	wxFFile* file;			// ассоциированный файл
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
#define ERROR_SERIAL				wxT("Невозможно работать с этим последовательным портом")
#define ERROR_SERIAL_OPEN			wxT("Невозможно открыть порт.\nОшибка: 0x")
#define ERROR_SERIAL_SET_PARAMETERS	wxT("Невозможно установить параметры порта.\nОшибка: 0x")
#define ERROR_SERIAL_READ			wxT("Ошибка чтения данных: 0x")
#define ERROR_UDP_OPEN				wxT("Ошибка открытия UDP-сокета: ")

#define ERROR_THREAD_CREATE			"Невозможно создать поток"
#define ERROR_THREAD_START			"Невозможно запустить поток"
