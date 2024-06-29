#include "FramesContainer.h"

// Конструктор
FramesContainer::FramesContainer()
{
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

// Добавить данные нового CAN-пакет с раскраской его данных в таблицу
void FramesContainer::AddFrame(CANFrameIn& frame)
{
	// поиск ID в таблице
	size_t idCount = frames.size();
	for (size_t iID = 0; iID < idCount; iID++)
	{
		// если найден - выделить яркостью цета изменяющиеся данные и заменить CAN-пакет
		if (frames[iID].frame.id == frame.id)
		{
			// обновить данные раскраски элементов
			for (size_t iData = 0; iData < frames[iID].frame.length; iData++)
			{
				// если новые данные такие же, что были ранее - необходимо плавно осветлять фоновую заливку
				if (frames[iID].frame.data[iData] == frame.data[iData])
				{
					if (frames[iID].color[iData] == NEW_COLOR)
					{
						frames[iID].color[iData] = CHANGE_COLOR;
					}
					else
					{
						// постепенно добавить цвет до белого в каналах G и B
						uint32_t curentColorValue = frames[iID].color[iData].GetRGBA();
						if (curentColorValue < DEFAULT_COLOR)
						{
							curentColorValue += 0x00010100;
							frames[iID].color[iData] = curentColorValue;
						}
					}						
				}
				else
				{
					frames[iID].color[iData] = CHANGE_COLOR;
				}
			}
			// обновить данные пакета
			frames[iID].frame = frame;
			return;
		}
	}

	// если ID не найден - добавить новый с зелёным цветом
	VisualCANFrame vFrame;
	vFrame.frame = frame;
	fill_n(vFrame.color, 8, NEW_COLOR);

	frames.push_back(vFrame);
	sort(frames.begin(), frames.end());
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
