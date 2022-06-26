#include "FIFOBuffer.h"

// Деструктор очищает очередь
FIFOBuffer::~FIFOBuffer()
{
	Clear();
}

// Вернуть размер очереди
size_t FIFOBuffer::Count()
{
	wxMutexLocker lock(syncFifoQueue);
	return data.size();
}

// Добавить один байт
void FIFOBuffer::Add(uint8_t value)
{
	wxMutexLocker lock(syncFifoQueue);
	data.push(value);
}

// Добавить массив байтов
void FIFOBuffer::Add(uint8_t values[], size_t amount)
{
	wxMutexLocker lock(syncFifoQueue);
	for (size_t index = 0; index < amount; index++)
	{
		data.push(values[index]);
	}
}

// Прочитать следующий байт с контролем наличия
uint8_t FIFOBuffer::Next(bool& ok)
{
	uint8_t value;
	syncFifoQueue.Lock();
	if (ok = !data.empty())
	{
		value = data.front();
		data.pop();
	}
	else
	{
		value = 0;
	}
	syncFifoQueue.Unlock();

	return value;
}

// Очистить буфер
void FIFOBuffer::Clear()
{
	wxMutexLocker lock(syncFifoQueue);
	while (!data.empty())
	{
		data.pop();
	}
}
