#include "CANParser.h"

// Собрать CAN-пакет из входящего потока байтов (контроль границ вне этого кода, должно быть не менее CAN_DATA_MINIMAL байт данных)
bool CANParser::Parse(uint8_t** bufferHead, CANFrameIn& frame)
{
    // поиск сигнатуры в потоке байтов
    if (*(uint32_t*)*bufferHead == SIG_DWORD)
    {
        *bufferHead += 4;
        // сборка пакета
        frame.id = *(uint32_t*)*bufferHead;
        *bufferHead += 4;
        frame.interval = *(uint16_t*)*bufferHead;
        *bufferHead += 2;
        frame.length = *(*bufferHead)++;
        if (frame.length <= 8)
        {
            for (size_t iData = 0; iData < frame.length; iData++)
                frame.data[iData] = *(*bufferHead)++;

            return true;
        }
    }

    (*bufferHead)++;
    return false;
}
