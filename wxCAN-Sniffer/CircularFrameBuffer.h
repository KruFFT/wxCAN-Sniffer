#pragma once

#include "Common.h"

class CircularFrameBuffer
{
public:
	CircularFrameBuffer(size_t windowSize);
	~CircularFrameBuffer();

	void Clear();
	void Add(float value);
	float* Frame();
	size_t FrameSize();

private:
	float* buffer = nullptr;		// массив данных
	size_t bufferSize;				// размер массива данных
	size_t frameSize;				// размер окна данных
	size_t frameBegin;				// начало окна данных
	size_t frameEnd;				// конец окна данных
	size_t frameSizeBytesCount;		// количество байтов в кадре
};
