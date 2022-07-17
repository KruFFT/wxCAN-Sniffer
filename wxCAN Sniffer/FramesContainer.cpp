#include "FramesContainer.h"

//  онструктор
FramesContainer::FramesContainer()
{
	frames.reserve(FRAMES_DATA_RESERV);
	frames.clear();
}

// ƒеструктор
FramesContainer::~FramesContainer()
{
	frames.clear();
}

// ќчистить хранилище CAN-пакетов
void FramesContainer::Clear()
{
	frames.clear();
}

// ƒобавить новый CAN-пакет с раскраской его данных
void FramesContainer::AddFrame(CANFrame& frame)
{
	// поиск ID в таблице
	size_t idCount = frames.size();
	for (size_t iID = 0; iID < idCount; iID++)
	{
		// если найден - выделить €ркостью цета измен€ющиес€ данные и заменить CAN-пакет
		if (frames[iID].Frame.ID == frame.ID)
		{
			// обновить данные раскраски элементов
			for (size_t iData = 0; iData < frames[iID].Frame.Length; iData++)
			{
				// если новые данные такие же, что были ранее - необходимо плавно осветл€ть фоновую заливку
				if (frames[iID].Frame.Data[iData] == frame.Data[iData])
				{
					if (frames[iID].Color[iData] == NEW_COLOR)
					{
						frames[iID].Color[iData] = CHANGE_COLOR;
					}
					else
					{
						// постепенно добавить цвет до белого в каналах G и B
						uint32_t curentColorValue = frames[iID].Color[iData].GetRGBA();
						if (curentColorValue < DEFAULT_COLOR)
						{
							curentColorValue += 0x00010100;
							frames[iID].Color[iData] = curentColorValue;
						}
					}						
				}
				else
				{
					frames[iID].Color[iData] = CHANGE_COLOR;
				}
			}
			// обновить данные пакета
			frames[iID].Frame = frame;
			return;
		}
	}

	// если ID не найден - добавить новый с зелЄным цветом
	VisualCANFrame vFrame;
	vFrame.Frame = frame;
	fill_n(vFrame.Color, 8, NEW_COLOR);

	frames.push_back(vFrame);
	sort(frames.begin(), frames.end());
}

// ¬ернуть размер хранилища
size_t FramesContainer::Size()
{
	return frames.size();
}

// ¬ернуть указанный отображаемый CAN-пакет
VisualCANFrame FramesContainer::GetFrame(size_t index)
{
	return frames[index];
}
