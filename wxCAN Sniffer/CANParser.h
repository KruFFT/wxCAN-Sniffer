#pragma once

#include "Common.h"

class CANParser
{
public:
	CANParser() = default;
	static bool Parse(uint8_t** bufferHead, VisualCANFrame& frame);
};
