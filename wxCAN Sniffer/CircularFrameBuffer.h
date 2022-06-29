#pragma once

#include "Common.h"

class CircularFrameBuffer
{
public:
	CircularFrameBuffer(size_t windowSize);
	~CircularFrameBuffer();

	void Clear();
	void Add(uint32_t value);
	uint32_t* Frame();
	size_t FrameSize();

private:
	uint32_t* buffer = nullptr;		// массив данных
	size_t bufferSize;				// размер массива данных
	size_t frameSize;				// размер окна данных
	size_t frameBegin;				// начало окна данных
	size_t frameEnd;				// конец окна данных
};
