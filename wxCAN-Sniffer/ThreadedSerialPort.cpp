#include "ThreadedSerialPort.h"

// Конструктор
ThreadedSerialPort::ThreadedSerialPort(wxString serialPort, uint32_t portSpeed, wxFrame* handleWindow) : wxThread(wxTHREAD_JOINABLE)
{
    // создать новый поток
    if (this->Create() != wxTHREAD_NO_ERROR)
    {
#ifdef __WINDOWS__
        throw new std::exception(ERROR_THREAD_CREATE);
#endif
#ifdef __LINUX__
        throw new std::exception();
#endif
#ifdef __APPLE__
        throw new std::exception();
#endif
    }

    portName = PORT_PREFIX + serialPort;
    baudRate = portSpeed;
    handleFrame = handleWindow;

    // запустить новый поток
    auto runError = this->Run();
    if (runError != wxTHREAD_NO_ERROR)
    {
#ifdef __WINDOWS__
        throw new std::exception(ERROR_THREAD_START);
#endif
#ifdef __LINUX__
        throw new std::exception();
#endif
#ifdef __APPLE__
        throw new std::exception();
#endif
    }
}

// Деструктор
ThreadedSerialPort::~ThreadedSerialPort()
{
    // удалить буфер приёма сообщений
    if (buffer)
    {
        delete[] buffer;
        buffer = nullptr;
    }
}

// Настройка параметров последовательного порта
bool ThreadedSerialPort::SetParameters()
{
#ifdef __WINDOWS__
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        return false;
    }
    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        return false;
    }

    COMMTIMEOUTS commTimeouts = { 0 };
    commTimeouts.ReadIntervalTimeout = MAXDWORD;
    commTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
    commTimeouts.ReadTotalTimeoutConstant = 0;
    commTimeouts.WriteTotalTimeoutMultiplier = 0;
    commTimeouts.WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(hSerial, &commTimeouts))
    {
        return false;
    }

    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
#endif

#ifdef __LINUX__
    // очистка всех флагов состояния
    if (fcntl(hSerial, F_SETFL, 0) < 0)
    {
        SendLastErrorMessage(ERROR_SERIAL_SET_PARAMETERS);
        close(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
        return false;
    }

    termios tty;

    if (tcgetattr(hSerial, &tty) < 0)
    {
        return false;
    }

    tty.c_cflag &= ~PARENB;                 // без проверки чётности
    tty.c_cflag &= ~INPCK;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                     // 8 бит данных
    tty.c_cflag &= ~CSTOPB;                 // 1 стоп-бит
    tty.c_cflag |= CLOCAL | CREAD;          // для чтения данных
    tty.c_cflag &= ~CRTSCTS;                // без контроля потока

    tty.c_oflag &= ~OPOST;                  // без спецсимволов
    tty.c_oflag &= ~ONLCR;                  // без перевода каретки

    tty.c_lflag &= ~ICANON;                 // не каноничный режим
    tty.c_lflag &= ~ECHO;                   // без эхо
    tty.c_lflag &= ~ECHOE;                  // без стирания через эхо
    tty.c_lflag &= ~ECHONL;                 // без перевода каретки в эхо
    tty.c_lflag &= ~ISIG;                   // без сигнальных символов

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // без программного контроля потока
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);    // без спецсимволов

    tty.c_cc[VTIME] = 1;                    // ожидание 100 мс (шаг по 100 мс)
    tty.c_cc[VMIN] = 19;                    // или ожидения 19 байтов в буфере

    tcflush(hSerial, TCIFLUSH);

    if (tcsetattr(hSerial, TCSANOW, &tty) < 0)
    {
        return false;
    }

    termios2 tty2;

    if (ioctl(hSerial, TCGETS2, &tty2) < 0)
    {
        return false;
    }

    tty2.c_cflag &= ~CBAUD;                 // очистить текущую скорость
    tty2.c_cflag |= BOTHER;                 // разрешить любую скорость

    tty2.c_ispeed = baudRate;               // задать скорость ввода
    tty2.c_ospeed = baudRate;               // задать скорость вывода

    if (ioctl(hSerial, TCSETS2, &tty2) < 0)
    {
        return false;
    }
#endif

#ifdef __APPLE__
    // очистка всех флагов состояния
    if (fcntl(hSerial, F_SETFL, 0) < 0)
    {
        SendLastErrorMessage(ERROR_SERIAL_SET_PARAMETERS);
        close(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
        return false;
    }

    termios tty;

    if (tcgetattr(hSerial, &tty) < 0)
    {
        return false;
    }

    tty.c_cflag &= ~PARENB;                 // без проверки чётности
    tty.c_cflag &= ~INPCK;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                     // 8 бит данных
    tty.c_cflag &= ~CSTOPB;                 // 1 стоп-бит
    tty.c_cflag |= CLOCAL | CREAD;          // для чтения данных
    tty.c_cflag &= ~CRTSCTS;                // без контроля потока

    tty.c_oflag &= ~OPOST;                  // без спецсимволов
    tty.c_oflag &= ~ONLCR;                  // без перевода каретки

    tty.c_lflag &= ~ICANON;                 // не каноничный режим
    tty.c_lflag &= ~ECHO;                   // без эхо
    tty.c_lflag &= ~ECHOE;                  // без стирания через эхо
    tty.c_lflag &= ~ECHONL;                 // без перевода каретки в эхо
    tty.c_lflag &= ~ISIG;                   // без сигнальных символов

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // без программного контроля потока
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);    // без спецсимволов

    tty.c_cc[VTIME] = 1;                    // ожидание 100 мс (шаг по 100 мс)
    tty.c_cc[VMIN] = 19;                    // или ожидания 19 байтов в буфере

    tcflush(hSerial, TCIFLUSH);

    if (tcsetattr(hSerial, TCSANOW, &tty) < 0)
    {
        return false;
    }

    speed_t speedBaudRate = (speed_t)baudRate;
    if (ioctl(hSerial, IOSSIOSPEED, &speedBaudRate, 1) < 0)
    {
        SendLastErrorMessage(ERROR_SERIAL_SET_PARAMETERS);
        close(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
        return false;
    }
#endif

    return true;
}

// Основной цикл потока
wxThread::ExitCode ThreadedSerialPort::Entry()
{
#ifdef __WINDOWS__
    DWORD  bytesRead = 0;       // количество прочитанных из последовательного порта данных
    DWORD  bytesSent;           // количество отправленных байтов
#endif
#ifdef __LINUX__
    int32_t  bytesRead = 0;     // количество прочитанных из последовательного порта данных
    uint32_t bytesSent;         // количество отправленных байтов
#endif
#ifdef __APPLE__
    int32_t  bytesRead = 0;     // количество прочитанных из последовательного порта данных
    uint32_t bytesSent;         // количество отправленных байтов
#endif  
    uint32_t bytesToSend;       // количество байтов для отправки
    uint8_t* bufferHead;        // указатель на начало данных в буфере
    uint8_t* bufferTail;        // указатель на конец данных в буфере
    uint8_t* bufferEnd;         // указатель на конец буфера для контроля
    
    uint32_t bufferFreeLength;  // размер свободного места в буфере 
    uint32_t dataLength;        // количество байтов с данными в буфере
    uint32_t freeLength;        // количество свободных байтов в буфере до начала данных в нём
    bool     readResult;        // результат чтения данных из последовательного порта

    // открытие порта и установка его параметров
#ifdef __WINDOWS__
    hSerial = CreateFileW(portName, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
#ifdef __LINUX__
    hSerial = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
#endif
#ifdef __APPLE__
    const char* asciiPortName = portName.mb_str().data();
    hSerial = open(asciiPortName, O_RDWR | O_NOCTTY | O_NDELAY | O_EXLOCK | O_NONBLOCK);
    //hSerial = open(asciiPortName, O_RDWR | O_NOCTTY | O_NDELAY);
#endif

    if (hSerial == INVALID_HANDLE_VALUE)
    {
        SendLastErrorMessage(ERROR_SERIAL_OPEN);
        Exit((wxThread::ExitCode)-1);
    }
    if (!SetParameters())
    {
        SendLastErrorMessage(ERROR_SERIAL_SET_PARAMETERS);
        Exit((wxThread::ExitCode)-1);
    }

    // выделить память под буфер и установка указателей в исходные состояния
    buffer = new uint8_t[BUFFER_SIZE + 1];
    bufferHead = bufferTail = buffer;
    bufferEnd = buffer + BUFFER_SIZE;

    // сообщить об успешном запуске
    handleFrame->GetEventHandler()->AddPendingEvent(wxThreadEvent(wxEVT_SERIAL_PORT_THREAD_STARTED));

    // проверка на запрос для выхода
    while (!TestDestroy())
    {
        // контроль границ буфера
        // если хвост буфера дошёл до конца буфера...
        if (bufferTail >= bufferEnd)
        {
            // и если голова буфера не сдвинулась с начала - данные не обрабатываются
            if (bufferHead == buffer)
            {
                // обнуление буфера, чтобы данные читались непрерывно
                bufferHead = bufferTail = buffer;
            }
            else
            {
                // необходимо сдвинуть буфер: скопировать данные массива в начало
                dataLength = bufferTail - bufferHead;
                freeLength = bufferHead - buffer;
                if (dataLength < freeLength)
                {
                    MEMCOPY(buffer, bufferHead, dataLength);
                    // коррекция указателей
                    bufferHead = buffer;
                    bufferTail = buffer + dataLength;
                }
                else
                {
                    throw;
                }
            }
        }

        // чтение данных из порта в хвост буфера
        bufferFreeLength = bufferEnd - bufferTail;
#ifdef __WINDOWS__
        readResult = ReadFile(hSerial, bufferTail, bufferFreeLength, &bytesRead, NULL);
#endif
#ifdef __LINUX__
        bytesRead = read(hSerial, bufferTail, bufferFreeLength);
        readResult = (bytesRead > 0);
#endif
#ifdef __APPLE__
        bytesRead = read(hSerial, bufferTail, bufferFreeLength);
        readResult = (bytesRead > 0);
#endif

        if (readResult)
        {
            bufferTail += bytesRead;
        }
        else
        {
            SendLastErrorMessage(ERROR_SERIAL_READ);
            break;
        }

        // поиск CAN-пакета и формирование данных
        while (bufferTail - bufferHead >= Parameters::can.MinimalDataSize)
        {
            CANFrameIn frame;

            // пакет собран - положить пакет в очередь
            if (CANParser::Parse(&bufferHead, frame))
            {
                syncCANBuffer.Lock();
                canBuffer.push(frame);
                syncCANBuffer.Unlock();

                // сгенерировать событие
                handleFrame->GetEventHandler()->AddPendingEvent(wxThreadEvent(wxEVT_SERIAL_PORT_THREAD_UPDATE));
            }
        }

        // если есть данные на отправку - отправить
        if (frameToSend.Frame.id)
        {
            syncCANSend.Lock();
            frameToSend.Signature = Parameters::can.Signature;
            // отправлять всегда весь пакет, все 19 байтов
            bytesToSend = sizeof(SendCANFrame);
#ifdef __WINDOWS__
            WriteFile(hSerial, &frameToSend, bytesToSend, &bytesSent, NULL);
#endif
#ifdef __LINUX__
            write(hSerial, &frameToSend, bytesToSend);
#endif
#ifdef __APPLE__
            write(hSerial, &frameToSend, bytesToSend);
#endif

            // после отправки - сбросить флаг (ID пакета) наличия данных
            frameToSend.Frame.id = 0;
            syncCANSend.Unlock();
        }
        /*else
        {
            // код для тестирования поиска данных в потоке мусора на стороне Arduino
            // если нет никаких данных на отправку - отправлять всякий мусор
            byte randomByte = rand() % 255;
            WriteFile(hSerial, &randomByte, 1, &bytesWritten, NULL);
        }*/
    }

    if (hSerial && hSerial != INVALID_HANDLE_VALUE)
    {
#ifdef __WINDOWS__
        CloseHandle(hSerial);
        hSerial = nullptr;
#endif
#ifdef __LINUX__
        close(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
#endif
#ifdef __APPLE__
        close(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
#endif
    }

    // удалить буфер приёма сообщений
    if (buffer)
    {
        delete[] buffer;
        buffer = nullptr;
    }

    // очистка буферов
    syncCANBuffer.Lock();
    while (!canBuffer.empty())
    {
        canBuffer.pop();
    }
    syncCANBuffer.Unlock();

    handleFrame->GetEventHandler()->AddPendingEvent(wxThreadEvent(wxEVT_SERIAL_PORT_THREAD_EXIT));
    return (wxThread::ExitCode)0;
}

// Возвращает очередной CAN-пакет из очереди
bool ThreadedSerialPort::GetNextFrame(CANFrameIn& frame)
{
    wxMutexLocker lock(syncCANBuffer);
    if (!canBuffer.empty())
    {
        frame = canBuffer.front();
        canBuffer.pop();
        return true;
    }
    return false;
}

// Отправить CAN-пакет
void ThreadedSerialPort::SendFrame(CANFrameOut& frame)
{
    // создаётся локальная копия пакета данных
    wxMutexLocker lock(syncCANSend);
    MEMCOPY(&frameToSend.Frame, &frame, sizeof(CANFrameOut));
    // если понадобится - поменять порядок байтов в идентификаторе
    //frameToSend.Frame.id = SWAP_BYTES_UINT32(frameToSend.Frame.id);
}

// Отправить сообщение об ошибке
void ThreadedSerialPort::SendLastErrorMessage(const wxChar* prefix)
{
    wxString errorMessage = prefix;
#ifdef __WINDOWS__
    uint32_t errorCode = GetLastError();
    errorMessage += wxString::Format(FORMAT_HEX8, errorCode);
#endif
#ifdef __LINUX__
    uint32_t errorCode = errno;
    errorMessage += wxString::Format(FORMAT_HEX8, errorCode) + wxT(" - ") + strerror(errorCode);
#endif
#ifdef __APPLE__
    uint32_t errorCode = errno;
    errorMessage += wxString::Format(FORMAT_HEX8, errorCode) + wxT(" - ") + strerror(errorCode);
#endif
    wxThreadEvent event(wxEVT_SERIAL_PORT_THREAD_MESSAGE);
    event.SetPayload(errorMessage);
    handleFrame->GetEventHandler()->AddPendingEvent(event);
}

// Возвращает список доступных последовательных портов
std::vector<ThreadedSerialPort::Information> ThreadedSerialPort::Enumerate()
{
    std::vector<Information> ports;

#ifdef __WINDOWS__
    HDEVINFO hDevicesInformation = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_COMPORT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevicesInformation != INVALID_HANDLE_VALUE)
    {
        SP_DEVINFO_DATA devicesInformation;
        devicesInformation.cbSize = sizeof(SP_DEVINFO_DATA);

        for (size_t index = 0; SetupDiEnumDeviceInfo(hDevicesInformation, index, &devicesInformation); index++)
        {
            ThreadedSerialPort::Information information;

            // название порта            
            auto hRegKey = SetupDiOpenDevRegKey(hDevicesInformation, &devicesInformation, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
            if (hRegKey != INVALID_HANDLE_VALUE)
            {
                wxChar port[256] = { 0 };
                DWORD dwSize = 255;
                auto registryResult = RegQueryValueExW(hRegKey, wxT("PortName"), NULL, NULL, (BYTE*)port, &dwSize);
                if (registryResult == ERROR_SUCCESS)
                {
                    RegCloseKey(hRegKey);
                    information.Port = port;

                    // полное название
                    wxChar description[256] = { 0 };
                    if (SetupDiGetDeviceRegistryPropertyW(hDevicesInformation, &devicesInformation, SPDRP_FRIENDLYNAME, NULL, (PBYTE)description, sizeof(description), NULL))
                    {
                        information.Description = description;
                    }

                    // идентификатор оборудования
                    wxChar hardwareId[256] = { 0 };
                    if (SetupDiGetDeviceRegistryPropertyW(hDevicesInformation, &devicesInformation, SPDRP_HARDWAREID, NULL, (PBYTE)hardwareId, sizeof(hardwareId), NULL))
                    {
                        information.HardwareID = hardwareId;
                    }

                    ports.push_back(information);
                }
            }
        }

        SetupDiDestroyDeviceInfoList(hDevicesInformation);
    }
#endif

#ifdef __LINUX__
    bool nonStandardPortFound = false;
    wxDir ttyDirectory(TTY_DIRECTORY);
    if (ttyDirectory.IsOpened())
    {
        wxString ttyName;
        auto hasNext = ttyDirectory.GetFirst(&ttyName);
        while (hasNext)
        {
            if (ttyName.compare(wxT(".")) && ttyName.compare(wxT("..")))
            {
                wxTextFile ttyFileDevice(TTY_DIRECTORY + ttyName + TTY_DEVICE);
                if (ttyFileDevice.Exists())
                {
                    ttyFileDevice.Open();
                    if (ttyFileDevice.IsOpened())
                    {
                        Information info;
                        info.Port = ttyName;
                        auto ttyFileLine = ttyFileDevice.GetFirstLine();
                        while (!ttyFileDevice.Eof())
                        {
                            if (ttyFileLine.StartsWith(TTY_DRIVER))
                            {
                                auto ttyDeviceName = ttyFileLine.AfterFirst(wxT('='));
                                if (!ttyDeviceName.IsEmpty())
                                {
                                    // если обнаружен "нестандартный" последовательный порт - убрать стандартные и оставлять только их
                                    if (!nonStandardPortFound && ttyDeviceName != TTY_PORT_DRIVER)
                                    {
                                        nonStandardPortFound = true;
                                        ports.clear();
                                    }
                                    info.Description = ttyDeviceName;
                                }
                            }
                            else if (ttyFileLine.StartsWith(TTY_MODALIAS))
                            {
                                auto ttyDeviceID = ttyFileLine.AfterFirst(wxT('='));
                                if (!ttyDeviceID.IsEmpty())
                                {
                                    wxString ids;
                                    auto hardwareID = ttyDeviceID.BeforeFirst(wxT(':'), &ids);
                                    hardwareID += wxT("/vid_") + ids.Left(5).Right(4);
                                    hardwareID += wxT("&pid_") + ids.Left(10).Right(4);
                                    hardwareID += wxT("&rev_") + ids.Left(15).Right(4);
                                    info.HardwareID = hardwareID;
                                }
                            }
                            ttyFileLine = ttyFileDevice.GetNextLine();
                        }
                        if (!info.Description.IsEmpty())
                        {
                            // фильтр стандартных портов, если найден любой другой
                            if (nonStandardPortFound)
                            {
                                if (info.Description != TTY_PORT_DRIVER)
                                {
                                    ports.push_back(info);
                                }
                            }
                            else
                            {
                                ports.push_back(info);
                            }
                        }
                    }
                }
            }
            hasNext = ttyDirectory.GetNext(&ttyName);
        }
    }
#endif

#ifdef __APPLE__
    // TODO: перечисление списка доступных портов
#endif

    sort(ports.begin(), ports.end());
    return ports;
}
