#pragma once

#include "Common.h"

class CANParser
{
public:
	CANParser() = default;
	static CANFrame Parse(uint8_t** bufferHead, bool* ok);
};
