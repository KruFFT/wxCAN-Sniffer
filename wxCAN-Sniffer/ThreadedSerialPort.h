#pragma once

#include "Common.h"
#include "CANParser.h"
#include <queue>

#ifdef __WINDOWS__
    #include <windows.h>
    #include <SetupAPI.h>
    #pragma comment(lib, "SetupAPI.lib")

    #define PORT_PREFIX wxT("\\\\.\\")
#endif

#ifdef __LINUX__
    #include <wx/dir.h>
    #include <wx/textfile.h>
    #include <stdio.h>
    #include <termios.h>
    #include <sys/ioctl.h>

    #ifndef BOTHER
    #define BOTHER 0010000
    #endif
    struct termios2
    {
        tcflag_t c_iflag;
        tcflag_t c_oflag;
        tcflag_t c_cflag;
        tcflag_t c_lflag;
        cc_t c_line;
        cc_t c_cc[19];
        speed_t c_ispeed;
        speed_t c_ospeed;
    };

    #define TTY_DIRECTORY           wxT("/sys/class/tty/")
    #define TTY_DEVICE              wxT("/device/uevent")
    #define TTY_DRIVER              wxT("DRIVER=")
    #define TTY_PORT_DRIVER         wxT("port")
    #define TTY_MODALIAS            wxT("MODALIAS=")
    #define DEV_DIRECTORY           wxT("/dev/")
    #define PORT_PREFIX             wxT("/dev/")
    #define INVALID_HANDLE_VALUE    -1
#endif

#ifdef __APPLE__
    #include <sys/ioctl.h>
    #include <sys/param.h>  
    #include <IOKit/IOKitLib.h>
    #include <IOKit/usb/IOUSBLib.h>
    #include <IOKit/serial/IOSerialKeys.h>
    #include <IOKit/serial/ioss.h>
    #include <CoreFoundation/CoreFoundation.h>

    #define DEV_DIRECTORY           wxT("/dev/")
    #define PORT_PREFIX             wxT("/dev/")
    #define INVALID_HANDLE_VALUE    -1
#endif

#define BUFFER_SIZE 1000000             // ёмкость буфера приёма данных

static wxMutex syncCANBuffer;
static wxMutex syncCANSend;

// События, которые генерирует этот поток
wxDECLARE_EVENT(wxEVT_SERIAL_PORT_THREAD_UPDATE, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_SERIAL_PORT_THREAD_EXIT, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_SERIAL_PORT_THREAD_MESSAGE, wxThreadEvent);

class ThreadedSerialPort : public wxThread
{
public:
    ThreadedSerialPort(wxString serialPort, uint32_t portSpeed, wxFrame* handleWindow);
    ~ThreadedSerialPort();

    bool GetNextFrame(CANFrameIn& frame);
    void SendFrame(CANFrameOut& frame);

#ifdef __WINDOWS__
    HANDLE hSerial = nullptr;           // хэндл открытого последовательного порта
#endif
#ifdef __LINUX__
    int hSerial = 0;
#endif
#ifdef __APPLE__
    int hSerial = 0;
#endif

    struct Information                  // структура описания последовательного порта
    {
    public:
        wxString Port;
        wxString Description;
        wxString HardwareID;        

        // оператор сравнения для естественной сортировки
        bool operator < (const Information& anotherInformation) const
        {
            return (Port.Length() == anotherInformation.Port.Length()) ?
                (Port < anotherInformation.Port) :
                (Port.Length() < anotherInformation.Port.Length());
        }
    };

    static std::vector<Information> Enumerate();

private:
    virtual ExitCode Entry();
    bool SetParameters();
    uint32_t SwapBytes(uint32_t value);
    void SendLastErrorMessage(const wxChar* prefix);

    wxString portName;                  // полное наименование последовательно порта с префиксами
    uint32_t baudRate;                  // указанная скорость
    wxFrame* handleFrame = nullptr;     // указатель на окно для генерации события для него
    uint8_t* buffer = nullptr;          // байтовый буфер последовательного порта
    std::queue<CANFrameIn> canBuffer;   // буфер полученных CAN-пакетов
    SendCANFrame frameToSend;           // CAN-пакет для отправки в CAN-сеть
};
