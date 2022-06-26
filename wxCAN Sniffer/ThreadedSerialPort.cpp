#include "ThreadedSerialPort.h"
#include "FIFOBuffer.h"

// Конструктор
ThreadedSerialPort::ThreadedSerialPort(wxString comPort, DWORD baudSpeed, wxFrame* windowHandle) : wxThread(wxTHREAD_JOINABLE)
{
	// создать новый поток
	if (this->Create() != wxTHREAD_NO_ERROR)
	{
		throw new exception("Невозможно создать поток");
	}

	portName = wxT("\\\\.\\") + comPort;
	baudRate = baudSpeed;
	handleFrame = windowHandle;

	// выделить память под буфер
	serialBuffer = new uint8_t[SERIAL_BUFFER_LEN + 1];

	// запустить новый поток
	wxThreadError runError = this->Run();
	if (runError != wxTHREAD_NO_ERROR)
	{
		throw new exception("Невозможно запустить поток");
	}

	/*FIFOBuffer buff;
	buff.Add(234);

	buff.Add(new uint8_t[4] {1, 2, 3, 4}, 4);

	bool ok;
	uint8_t testValue;
	testValue = buff.Next(ok);
	testValue = buff.Next(ok);
	testValue = buff.Next(ok);
	testValue = buff.Next(ok);
	testValue = buff.Next(ok);
	testValue = buff.Next(ok);*/
}

// Деструктор
ThreadedSerialPort::~ThreadedSerialPort()
{
	buffer.Clear();
	// удалить буфер
	delete[] serialBuffer;
	serialBuffer = NULL;
}

// Основной цикл потока
wxThread::ExitCode ThreadedSerialPort::Entry()
{
	DWORD bytesRead = 0;		// количество прочитанных из последовательного порта данных
	DWORD bytesToSend;			// количество байтов для отправки
	DWORD bytesSent;			// количество отправленных байтов
	uint8_t  step = 0;			// стадия сборки пакета из байтового потока
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

		// TODO установка указателей в исходные состояния
		//bufferHead = bufferTail = buffer;
		//bufferEnd = buffer + BUFFERLEN;

		// проверка на запрос для выхода
		while (!TestDestroy())
		{
			// контроль границ буфера
			// если хвост буфера дошёл до конца буфера...
			/*if (bufferTail >= bufferEnd)
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
			}*/

			// теперь чтение данных из порта в хвост буфера
			//bufferFreeLength = bufferEnd - bufferTail;
			readError = ReadFile(hSerial, serialBuffer, SERIAL_BUFFER_LEN, &bytesRead, NULL);
			if (readError)
			{
				buffer.Add(serialBuffer, bytesRead);

				parser.ParseAll(buffer);
			}
			else
			{
				// встретил только ошибку отключения порта (0x5), поэтому выход из цикла
				//DWORD lastError = GetLastError();
				break;
			}

			// поиск CAN-пакета и формирование данных
			/*while (bufferHead < bufferTail)
			{
				switch (step)
				{
					// искомая сигнатура в буфере 0xAA55AA55 - это начало пакета, байт 0 = 0xAA
					case 0:
						if (*bufferHead == SIG_BYTE_0)
						{
							step++;
						}
						else
						{
							step = 0;
						}
						bufferHead++;
						break;

					// поиск сигнатуры, байт 1 = 0x55
					case 1:
						if (*bufferHead == SIG_BYTE_1)
						{
							step++;
							bufferHead++;
						}
						else
						{
							step = 0;
						}
						break;

					// поиск сигнатуры, байт 2 = 0xAA
					case 2:
						if (*bufferHead == SIG_BYTE_2)
						{
							step++;
							bufferHead++;
						}
						else
						{
							step = 0;
						}
						break;

					// поиск сигнатуры, байт 3 = 0x55
					case 3:
						if (*bufferHead == SIG_BYTE_3)
						{
							step++;
							bufferHead++;
						}
						else
						{
							step = 0;
						}
						break;

					// ID пакета (байт 0)
					case 4:
						frame = { 0 };
						frame.ID = *bufferHead;
						bufferHead++;
						step++;
						break;

					// ID пакета (байт 1)
					case 5:
						frame.ID |= (*bufferHead) << 8;
						bufferHead++;
						step++;
						break;

					// ID пакета (байт 2)
					case 6:
						frame.ID |= (*bufferHead) << 16;
						bufferHead++;
						step++;
						break;

					// ID пакета (байт 3)
					case 7:
						frame.ID |= (*bufferHead) << 24;
						bufferHead++;
						step++;
						break;

					// длина пакета
					case 8:
						frame.Length = *bufferHead;
						bufferHead++;
						step++;
						if (frame.Length > 8)
						{
							// данные некорректные - возврат к поиску заголовка
							step = 0;
						}
						break;

					// данные пакета
					case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16:
						if (step - 9 < frame.Length)
						{
							frame.Data[step - 9] = *bufferHead;
							bufferHead++;
							step++;
						}
						else
						{
							step = 17;
						}
						break;

					// пакет собран - положить пакет в очередь
					case 17:
						syncCANBuffer.Lock();
						canBuffer.push(frame);
						syncCANBuffer.Unlock();
						step = 0;

						// сгенерировать событие
						wxQueueEvent(handleFrame, new wxThreadEvent(wxEVT_THREAD));
						break;
				}
			}*/

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
	}

	return FALSE;
}

// Возвращает очередной CAN-пакет из очереди
CANFrame ThreadedSerialPort::GetNextFrame(bool& ok)
{
	return parser.Next(ok);
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
