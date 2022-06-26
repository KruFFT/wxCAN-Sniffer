#pragma once

#include "Common.h"
#include "FIFOBuffer.h"
#include "CANParser.h"

#define SERIAL_BUFFER_LEN	100000
#define READ_PAUSE	20

//static wxMutex syncCANBuffer;
static wxMutex syncCANSend;

class ThreadedSerialPort : public wxThread
{
public:
	ThreadedSerialPort(wxString comPort, DWORD baudSpeed, wxFrame* windowHandle);
	~ThreadedSerialPort();

	CANFrame GetNextFrame(bool& ok);
	void SendFrame(CANFrame& frame);

	HANDLE hSerial = INVALID_HANDLE_VALUE;	// хэндл открытого последовательного порта

private:
	virtual void* Entry();
	uint32_t SwapBytes(uint32_t value);

	wxString portName;					// полное наименование последовательно порта с префиксами
	DWORD    baudRate;					// заданная скорость
	wxFrame* handleFrame = NULL;		// указатель на окно программы для генерации события для него

	FIFOBuffer buffer;					// байтовый буфер всех данных
	CANParser parser;					// парсер принятых данных
	uint8_t* serialBuffer;				// байтовый буфер чтения из поледовательного порта
	SendCANFrame frameToSend;			// CAN-пакет для отправки в CAN-сеть
};
