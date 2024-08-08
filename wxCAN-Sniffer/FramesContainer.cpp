#include "FramesContainer.h"

// Конструктор
FramesContainer::FramesContainer(WindowColors colors)
{
    themeColors = colors;
    frames.reserve(FRAMES_DATA_RESERV);
    frames.clear();
}

// Деструктор
FramesContainer::~FramesContainer()
{
    frames.clear();
}

// Очистить хранилище CAN-пакетов
void FramesContainer::Clear()
{
    frames.clear();
}

// Вернуть размер хранилища
size_t FramesContainer::Size()
{
    return frames.size();
}

// Вернуть указанный отображаемый CAN-пакет
VisualCANFrame FramesContainer::GetFrame(size_t index)
{
    return frames[index];
}

// Добавить данные нового CAN-пакет с раскраской его данных в таблицу
void FramesContainer::AddFrame(CANFrameIn& frame)
{
    size_t idCount = frames.size();
    for (size_t iID = 0; iID < idCount; iID++)
    {
        auto& currentFrame = frames[iID];
        // поиск ID пакета в таблице
        if (currentFrame.frame.id == frame.id)
        {   
            // если ID пакета найден - проверка всех его данных по очереди
            for (size_t iData = 0; iData < currentFrame.frame.length; iData++)
            {
                // если новый байт данных совпадают с предыдущими...
                if (currentFrame.frame.data[iData] == frame.data[iData])
                {
                    // если пакет ранее был новым...
                    if (currentFrame.ticks[iData] == NEW_PACKET)
                    {
                        // ... пометить его, как обновлённый
                        currentFrame.color[iData] = themeColors.GridUpdateBackground;
                        currentFrame.ticks[iData] = UPDATED_PACKET;
                    }
                }
                else
                {
                    currentFrame.color[iData] = themeColors.GridUpdateBackground;
                    currentFrame.ticks[iData] = UPDATED_PACKET;
                }
            }
            // обновить данные пакета
            currentFrame.frame = frame;
            return;
        }
    }

    // если ID не найден - добавить с цветом нового пакета
    VisualCANFrame vFrame;
    vFrame.frame = frame;
    std::fill_n(vFrame.color, 8, themeColors.GridNewBackground);
    uint8_t defaultShift = NEW_PACKET;
    std::fill_n(vFrame.ticks, 8, defaultShift);
    frames.push_back(vFrame);
    std::sort(frames.begin(), frames.end());
}

// Обработать такты всех пакетов
void FramesContainer::ProcessAllFrames()
{
    size_t idCount = frames.size();
    for (size_t iID = 0; iID < idCount; iID++)
    {
        auto& currentFrame = frames[iID];

        // цикл по всем байтам пакета
        for (size_t iData = 0; iData < currentFrame.frame.length; iData++)
        {
            // постепенно менять яркость для достижения фонового цвета по интервалу времени
            if (currentFrame.ticks[iData] >= UPDATED_PACKET && currentFrame.ticks[iData] < TIMEOUT_PACKET)
            {
                currentFrame.ticks[iData]++;
            }
        }
    }
}
