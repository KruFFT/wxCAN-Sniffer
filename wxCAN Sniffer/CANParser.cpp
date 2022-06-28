#include "CANParser.h"

// Собрать CAN-пакет из входящего потока байтов (контроль границ вне этого кода, должно быть не менее 17 байт данных)
bool CANParser::Parse(uint8_t** bufferHead, CANFrame& frame)
{
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

            return true;
        }
    }

    (*bufferHead)++;
    return false;
}
