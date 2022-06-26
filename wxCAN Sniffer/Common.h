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
#define COM_NAME	wxT("COM3")

#define SIG_BYTE_0 0xAA
#define SIG_BYTE_1 0x55
#define SIG_BYTE_2 0xAA
#define SIG_BYTE_3 0x55

#define UDP_PORT 0xAA55

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
	uint8_t Signature0 = SIG_BYTE_0;
	uint8_t Signature1 = SIG_BYTE_1;
	uint8_t Signature2 = SIG_BYTE_2;
	uint8_t Signature3 = SIG_BYTE_3;
	CANFrame Frame = { 0 };		// пакет для отправки
};

// Описание log-файла
struct LogFile
{
public:
	uint32_t ID;		// идентификатор пакета
	wxFFile* File;		// хэндл ассоциированного файла
};
