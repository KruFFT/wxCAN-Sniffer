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

#define CAPTION		wxT("CAN Sniffer 2.0.0")

#define SIG_BYTE_0	0xAA
#define SIG_BYTE_1	0x55
#define SIG_BYTE_2	0xAA
#define SIG_BYTE_3	0x55
#define SIG_DWORD	(uint32_t)(SIG_BYTE_3 << 24 | SIG_BYTE_2 << 16 | SIG_BYTE_1 << 8 | SIG_BYTE_0)

// CAN-пакет данных
struct CANFrame
{
public:
	uint32_t ID;		// идентификатор пакета
	uint8_t  Length;	// длина пакета
	uint8_t  Data[8];	// массив данных пакета, до 8 байт

	uint8_t  Tick[8];	// счётчик тактов появления пакета, используется для выделения цветом в таблице

	// оператор сравнения CAN-пакета необходим для сортировки
	bool operator < (const CANFrame& frame) const
	{
		return (ID < frame.ID);
	}
};

// CAN-пакет для отправки данных
struct SendCANFrame
{
public:
	uint32_t Signature = SIG_DWORD;
	CANFrame Frame = { 0 };		// пакет для отправки
};

// Описание log-файла
struct LogFile
{
public:
	uint32_t ID;		// идентификатор пакета
	wxFFile* File;		// хэндл ассоциированного файла
};

// Описание одного байтового буфера
struct Buffer
{
public:
	uint8_t* Pointer;
	size_t   Size;
};
