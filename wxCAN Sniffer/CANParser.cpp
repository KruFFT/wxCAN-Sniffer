#include "CANParser.h"

// Деструктор очищает очередь
CANParser::~CANParser()
{
	wxMutexLocker lock(syncCanQueue);
	while (!frames.empty())
	{
		frames.pop();
	}
}

void CANParser::ParseAll(FIFOBuffer& buffer)
{
    bool ok;
    CANFrame frame;

	while (buffer.Count() > 17)
	{
        if (buffer.Next(ok) == SIG_BYTE_0 &&
            buffer.Next(ok) == SIG_BYTE_1 &&
            buffer.Next(ok) == SIG_BYTE_2 &&
            buffer.Next(ok) == SIG_BYTE_3)
        {
            // сборка ID пакета
            uint8_t id0 = buffer.Next(ok);
            uint8_t id1 = buffer.Next(ok);
            uint8_t id2 = buffer.Next(ok);
            uint8_t id3 = buffer.Next(ok);
            frame.ID = id3 << 24 | id2 << 16 | id1 << 8 | id0;
            frame.Length = buffer.Next(ok);
            if (frame.Length <= 8)
            {
                for (size_t iData = 0; iData < frame.Length; iData++)
                    frame.Data[iData] = buffer.Next(ok);

                syncCanQueue.Lock();
                frames.push(frame);
                syncCanQueue.Unlock();
            }
        }
	}
}

CANFrame CANParser::Next(bool& ok)
{
    CANFrame value;
    syncCanQueue.Lock();
    if (ok = !frames.empty())
    {
        value = frames.front();
        frames.pop();
    }
    else
    {
        value.ID = 0;
    }
    syncCanQueue.Unlock();

    return value;
}