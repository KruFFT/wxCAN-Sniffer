#include "ThreadedSerialPort.h"

// Конструктор
ThreadedSerialPort::ThreadedSerialPort(wxString PortName, DWORD BaudRate, wxFrame* HandleFrame) : wxThread(wxTHREAD_JOINABLE)
{
	// создать новый поток
	if (this->Create() != wxTHREAD_NO_ERROR)
	{
		throw new exception("Невозможно создать поток");
	}

	portName = wxT("\\\\.\\") + PortName;
	baudRate = BaudRate;
	handleFrame = HandleFrame;

	// выделить память под буфер
	buffer = new byte[BUFFERLEN + 1];

	// запустить новый поток
	wxThreadError runError = this->Run();
	if (runError != wxTHREAD_NO_ERROR)
	{
		throw new exception("Невозможно запустить поток");
	}
}

// Деструктор
ThreadedSerialPort::~ThreadedSerialPort()
{
	// удалить буфер
	delete[] buffer;
	buffer = NULL;
}

// Основной цикл потока
wxThread::ExitCode ThreadedSerialPort::Entry()
{
	uint8_t* bufferHead;		// указатель на начало данных в буфере
	uint8_t* bufferTail;		// указатель на конец данных в буфере
	uint8_t* bufferEnd;			// указатель на конец буфера для контроля
	DWORD bytesRead = 0;		// количество прочитанных из последовательного порта данных
	DWORD bytesToSend;			// количество байтов для отправки
	DWORD bytesSent;			// количество отправленных байтов
	DWORD bufferFreeLength;		// размер свободного места в буфере	
	uint32_t dataLength;		// количество байтов с данными в буфере
	uint32_t freeLength;		// количество свободных байтов в буфере до начала данных в нём
	BOOL readError;				// результат чтения данных из последовательного порта

	hSerial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hSerial != INVALID_HANDLE_VALUE)
	{
		DCB dcbSerialParams = { 0 };
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		if (!GetCommState(hSerial, &dcbSerialParams))
		{
			throw new exception("Невозможно получить параметры порта");
		}
		dcbSerialParams.BaudRate = baudRate;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = NOPARITY;
		if (!SetCommState(hSerial, &dcbSerialParams))
		{
			throw new exception("Невозможно установить параметры порта");
		}

		COMMTIMEOUTS commTimeouts;
		commTimeouts.ReadIntervalTimeout = MAXDWORD;
		commTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
		commTimeouts.ReadTotalTimeoutConstant = 0;
		commTimeouts.WriteTotalTimeoutMultiplier = 0;
		commTimeouts.WriteTotalTimeoutConstant = 0;

		if (!SetCommTimeouts(hSerial, &commTimeouts))
		{
			throw new exception("Невозможно установить тайм-ауты порта");
		}

		PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

		// установка указателей в исходные состояния
		bufferHead = bufferTail = buffer;
		bufferEnd = buffer + BUFFERLEN;

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
						memcpy_s(buffer, freeLength, bufferHead, dataLength);
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

			// теперь чтение данных из порта в хвост буфера
			bufferFreeLength = bufferEnd - bufferTail;
			readError = ReadFile(hSerial, bufferTail, bufferFreeLength, &bytesRead, NULL);
			if (readError)
			{
				bufferTail += bytesRead;
			}
			else
			{
				// встретил только ошибку отключения порта (0x5), поэтому выход из цикла
				//DWORD lastError = GetLastError();
				break;
			}

			// поиск CAN-пакета и формирование данных
			//while (bufferHead < bufferTail)
			while (bufferTail - bufferHead >= 17)
			{
				bool ok;
				CANFrame frame = CANParser::Parse(&bufferHead, &ok);

				// пакет собран - положить пакет в очередь
				if (ok)
				{
					syncCANBuffer.Lock();
					canBuffer.push(frame);
					syncCANBuffer.Unlock();

					// сгенерировать событие
					wxQueueEvent(handleFrame, new wxThreadEvent(wxEVT_THREAD));
				}
			}

			// если есть данные на отправку - отправить
			if (frameToSend.Frame.ID != 0)
			{
				syncCANSend.Lock();
				// 4 байта сигнатуры + 4 байта ID-пакета + 1 байт длина данных + сами данные
				bytesToSend = 9 + frameToSend.Frame.Length;
				WriteFile(hSerial, &frameToSend, bytesToSend, &bytesSent, NULL);
				// после отправки - сбросить флаг наличия данных
				frameToSend.Frame.ID = 0;
				syncCANSend.Unlock();
			}
			/*else
			{
				// код для тестирования поиска данных в потоке мусора на стороне Arduino
				// если нет никаких данных на отправку - отправлять всякий мусор
				byte randomByte = rand() % 255;
				WriteFile(hSerial, &randomByte, 1, &bytesWritten, NULL);
			}*/

			// пауза потока после разбора всех данных для заполнения входного буфера данными
			Sleep(READ_PAUSE);
		}

		if (hSerial && hSerial != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hSerial);
			hSerial = NULL;
		}

		// очистка буферов
		syncCANBuffer.Lock();
		while (canBuffer.size() > 0)
		{
			canBuffer.pop();
		}
		syncCANBuffer.Unlock();
	}

	return FALSE;
}

// Возвращает очередной CAN-пакет из очереди
bool ThreadedSerialPort::GetNextFrame(CANFrame& frame)
{
	wxMutexLocker lock(syncCANBuffer);
	if (canBuffer.size() > 0)
	{
		frame = canBuffer.front();
		canBuffer.pop();
		return true;
	}
	return false;
}

// Отправить CAN-пакет
void ThreadedSerialPort::SendFrame(CANFrame& frame)
{
	// создаётся локальная копия пакета данных
	wxMutexLocker lock(syncCANSend);
	memcpy_s(&frameToSend.Frame, sizeof(CANFrame), &frame, sizeof(CANFrame));
	// если понадобится - поменять порядок байтов в идентификаторе
	//frameToSend.Frame.ID = SwapBytes(frameToSend.Frame.ID);
}

// Меняет порядок байтов в слове
uint32_t ThreadedSerialPort::SwapBytes(uint32_t value)
{
	uint32_t revValue = value & 0xFF;
	revValue = (revValue << 8) | ((value >> 8) & 0xFF);
	revValue = (revValue << 8) | ((value >> 16) & 0xFF);
	revValue = (revValue << 8) | ((value >> 24) & 0xFF);
	return revValue;
}
