#include "CANParser.h"

// Собрать CAN-пакет из входящего потока байтов (контроль границ вне этого кода, должно быть не менее 17 байт данных)
bool CANParser::Parse(uint8_t** bufferHead, VisualCANFrame& frame)
{
    // поиск сигнатуры в потоке байтов
    if (*(uint32_t*)*bufferHead == SIG_DWORD)
    {
        *bufferHead += 4;
        // сборка пакета
        frame.Frame.ID = *(uint32_t*)*bufferHead;
        *bufferHead += 4;
        frame.Frame.Length = *(*bufferHead)++;
        if (frame.Frame.Length <= 8)
        {
            for (size_t iData = 0; iData < frame.Frame.Length; iData++)
                frame.Frame.Data[iData] = *(*bufferHead)++;

            return true;
        }
    }

    (*bufferHead)++;
    return false;
}
