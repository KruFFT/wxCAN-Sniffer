#pragma once

#include "Common.h"
#include "FIFOBuffer.h"
#include <queue>

static wxMutex syncCanQueue;

class CANParser
{
public:
	CANParser() = default;
	~CANParser();

	void ParseAll(FIFOBuffer& buffer);
	CANFrame Next(bool& ok);

private:
	std::queue<CANFrame> frames;	// очередь с полученными пакетами
};

