#include "CANParser.h"

// Собрать CAN-пакет из входящего потока байтов (контроль границ вне этого кода, должно быть не менее 17 байт данных)
CANFrame CANParser::Parse(uint8_t** bufferHead, bool* ok)
{
    CANFrame frame = { 0 };
    *ok = false;

    // ниже "магия" указателей на указатели :)
    // поиск сигнатуры в потоке байтов
    if (*(uint32_t*)*bufferHead == SIG_DWORD)
    {
        *bufferHead += 4;
        // сборка пакета
        frame.ID = *(uint32_t*)*bufferHead;
        *bufferHead += 4;
        frame.Length = *(*bufferHead)++;
        if (frame.Length <= 8)
        {
            for (size_t iData = 0; iData < frame.Length; iData++)
                frame.Data[iData] = *(*bufferHead)++;

            *ok = true;
        }
    }
    else
    {
        (*bufferHead)++;
    }
    return frame;
}
