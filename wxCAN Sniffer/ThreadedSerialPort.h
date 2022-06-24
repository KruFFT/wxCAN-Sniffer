#pragma once

#include "Common.h"

#define BUFFERLEN	100000
#define READ_PAUSE	20

static wxMutex syncCANBuffer;
static wxMutex syncCANSend;

class ThreadedSerialPort : public wxThread
{
public:
	ThreadedSerialPort(wxString comPort, DWORD baudSpeed, wxFrame* windowHandle);
	~ThreadedSerialPort();

	bool GetNextFrame(CANFrame* frame);
	void SendFrame(CANFrame* frame);

	HANDLE hSerial = INVALID_HANDLE_VALUE;	// хэндл открытого последовательного порта

private:
	virtual void* Entry();
	uint32_t SwapBytes(uint32_t value);

	wxString portName;					// полное наименование последовательно порта с префиксами
	DWORD    baudRate;					// указанная скорость
	wxFrame* handleFrame = NULL;		// указатель на окно для генерации события для него

	uint8_t* buffer;					// байтовый буфер поледовательного порта
	queue<CANFrame> canBuffer;			// буфер полученных CAN-пакетов
	SendCANFrame frameToSend;			// CAN-пакет для отправки в CAN-сеть
};
