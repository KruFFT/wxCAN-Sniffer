#pragma once

#include "Common.h"
#include "CANParser.h"

#define BUFFERLEN	1000000

static wxMutex syncCANBuffer;
static wxMutex syncCANSend;

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

	uint8_t* buffer;					// байтовый буфер поледовательного порта
	queue<CANFrame> canBuffer;			// буфер полученных CAN-пакетов
	SendCANFrame frameToSend;			// CAN-пакет для отправки в CAN-сеть
};
