#pragma once

#include "Common.h"
#include "CANParser.h"
#include <queue>
#include <windows.h>
#include <SetupAPI.h>
#pragma comment(lib, "SetupAPI.lib")

#define BUFFER_SIZE	1000000				// ёмкость буфера приёма данных

static wxMutex syncCANBuffer;
static wxMutex syncCANSend;

// События, которые генерирует этот поток
wxDECLARE_EVENT(wxEVT_SERIAL_PORT_THREAD_UPDATE, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_SERIAL_PORT_THREAD_EXIT, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_SERIAL_PORT_THREAD_MESSAGE, wxThreadEvent);

class ThreadedSerialPort : public wxThread
{
public:
	ThreadedSerialPort(wxString serialPort, DWORD portSpeed, wxFrame* handleWindow);
	~ThreadedSerialPort();

	bool GetNextFrame(CANFrameIn& frame);
	void SendFrame(CANFrameOut& frame);

	HANDLE hSerial = nullptr;			// хэндл открытого последовательного порта

	struct Information					// структура описания последовательного порта
	{
	public:
		wxString Port;
		wxString HardwareID;
		wxString Description;
	};

	static std::vector<Information> Enumerate();

private:
	virtual ExitCode Entry();
	bool SetParameters();
	uint32_t SwapBytes(uint32_t value);
	void SendLastErrorMessage(const wxChar* prefix);

	wxString portName;					// полное наименование последовательно порта с префиксами
	DWORD    baudRate;					// указанная скорость
	wxFrame* handleFrame = nullptr;		// указатель на окно для генерации события для него
	uint8_t* buffer = nullptr;			// байтовый буфер последовательного порта
	std::queue<CANFrameIn> canBuffer;	// буфер полученных CAN-пакетов
	SendCANFrame frameToSend;			// CAN-пакет для отправки в CAN-сеть
};
