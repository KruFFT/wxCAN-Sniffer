#include "CircularFrameBuffer.h"

// Конструктор
CircularFrameBuffer::CircularFrameBuffer(size_t windowSize)
{
	// размер буфера должен быть минимум в два раза больше размера окна
	bufferSize = windowSize * 4;
	buffer = new float[bufferSize + 1] { 0 };
	frameSize = windowSize;
	frameBegin = 0;
	frameEnd = frameSize;
	frameSizeBytesCount = bufferSize * sizeof(float);
}

// Деструктор
CircularFrameBuffer::~CircularFrameBuffer()
{
	if (buffer)
	{
		delete[] buffer;
		buffer = nullptr;
	}
}

// Очистить буфер
void CircularFrameBuffer::Clear()
{
	memset(buffer, 0, frameSizeBytesCount);
	frameBegin = 0;
	frameEnd = frameSize;
}

// Добавить новое значение со сдвигом при необходимости
void CircularFrameBuffer::Add(float value)
{
	// если достигнут предел массива - надо сдвинуть данные
	if (frameEnd >= bufferSize)
	{
		MEMCOPY(buffer, &buffer[frameBegin], frameSizeBytesCount);
		frameBegin = 0;
		frameEnd = frameSize;
	}
	// добавить значение
	buffer[frameEnd] = value;
	frameBegin++;
	frameEnd++;
}

// Вернуть указатель на начало кадра
float* CircularFrameBuffer::Frame()
{
	return &buffer[frameBegin];
}

// Вернуть размер кадра
size_t CircularFrameBuffer::FrameSize()
{
	return frameSize;
}
