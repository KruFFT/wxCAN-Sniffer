#pragma once

#include <wx/thread.h>
#include <stdint.h>
#include <queue>

static wxMutex syncFifoQueue;

class FIFOBuffer
{
public:
	FIFOBuffer() = default;
	~FIFOBuffer();
	size_t Count();
	void Add(uint8_t value);
	void Add(uint8_t values[], size_t amount);
	uint8_t Next(bool& ok);
	void Clear();

private:
	std::queue<uint8_t> data;		// очередь с данными
};
