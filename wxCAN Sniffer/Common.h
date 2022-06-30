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

// CAN-пакет для отправки данных
#pragma pack (push, 1)
struct CANFrame
{
public:
	uint32_t ID;		// идентификатор пакета
	uint8_t  Length;	// длина пакета
	uint8_t  Data[8];	// массив данных пакета, до 8 байт
};
#pragma pack(pop)

// CAN-пакет для отображения в таблице
#pragma pack (push, 1)
struct VisualCANFrame
{
public:
	CANFrame Frame = { 0 };	// пакет с данными
	uint8_t  Tick[8];		// счётчик тактов появления пакета, используется для выделения цветом в таблице

	// оператор сравнения CAN-пакета необходим для сортировки
	bool operator < (const VisualCANFrame& frame) const
	{
		return (Frame.ID < frame.Frame.ID);
	}
};
#pragma pack(pop)

// CAN-пакет для отправки данных
#pragma pack (push, 1)
struct SendCANFrame
{
public:
	uint32_t Signature = SIG_DWORD;
	CANFrame Frame = { 0 };		// пакет для отправки
};
#pragma pack(pop)

// Описание log-файла
struct LogFile
{
public:
	uint32_t ID;		// идентификатор пакета
	wxFFile* File;		// хэндл ассоциированного файла
};
