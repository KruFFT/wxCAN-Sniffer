#pragma once

#include <wx/wxprec.h>
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

#define CAPTION		wxT("CAN Sniffer 2.1.0")

#define SIG_BYTE_0	0xAA
#define SIG_BYTE_1	0x55
#define SIG_BYTE_2	0xAA
#define SIG_BYTE_3	0x55
#define SIG_DWORD	(uint32_t)(SIG_BYTE_3 << 24 | SIG_BYTE_2 << 16 | SIG_BYTE_1 << 8 | SIG_BYTE_0)

#define COM_NAME			wxT("COM6")	// последовательный порт по умолчанию
#define UDP_PORT			0xAA55		// UDP порт
#define UDP_BUFFER_SIZE		1000		// размер буфера приёма пакетов

// CAN-пакет для отправки данных
#pragma pack (push, 1)
struct CANFrame
{
public:
	uint32_t id;		// идентификатор пакета
	uint16_t interval;	// интервал между пакетами (мс)
	uint8_t  length;	// длина пакета
	uint8_t  data[8];	// массив данных пакета, до 8 байт
};
#pragma pack(pop)

// CAN-пакет для отображения в таблице
#pragma pack (push, 1)
struct VisualCANFrame
{
public:
	CANFrame frame;			// пакет с данными
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
struct SendCANFrame
{
public:
	uint32_t Signature = SIG_DWORD;
	CANFrame Frame;			// пакет для отправки
};
#pragma pack(pop)

// Описание log-файла
struct LogFile
{
public:
	uint32_t id;			// идентификатор пакета
	wxFFile* file;			// хэндл ассоциированного файла
};
