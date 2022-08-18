#pragma once

#include "Common.h"
#include "CANParser.h"

#define BUFFERLEN	1000000				// ёмкость буфера приёма данных

static wxMutex syncCANBuffer;
static wxMutex syncCANSend;

// События, которые генерирует этот поток
wxDECLARE_EVENT(wxEVT_SERIAL_PORT_THREAD_UPDATE, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_SERIAL_PORT_THREAD_EXIT, wxThreadEvent);

class ThreadedSerialPort : public wxThread
{
public:
	ThreadedSerialPort(wxString serialPort, DWORD portSpeed, wxFrame* handleWindow);
	~ThreadedSerialPort();

	bool GetNextFrame(CANFrame& frame);
	void SendFrame(CANFrame& frame);

	HANDLE hSerial = INVALID_HANDLE_VALUE;	// хэндл открытого последовательного порта

private:
	virtual void* Entry();
	uint32_t SwapBytes(uint32_t value);

	wxString portName;					// полное наименование последовательно порта с префиксами
	DWORD    baudRate;					// указанная скорость
	wxFrame* handleFrame = nullptr;		// указатель на окно для генерации события для него

	uint8_t* buffer = nullptr;			// байтовый буфер поледовательного порта
	queue<CANFrame> canBuffer;			// буфер полученных CAN-пакетов
	SendCANFrame frameToSend;			// CAN-пакет для отправки в CAN-сеть
};
