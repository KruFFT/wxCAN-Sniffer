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

// Добавить данные нового CAN-пакет с раскраской его данных в таблицу
void FramesContainer::AddFrame(CANFrameIn& frame)
{
	auto ms = wxGetUTCTimeMillis();

	// поиск ID в таблице
	size_t idCount = frames.size();
	for (size_t iID = 0; iID < idCount; iID++)
	{
		// если найден - выделить яркостью цвета изменяющиеся данные и заменить CAN-пакет
		if (frames[iID].frame.id == frame.id)
		{
			// обновить данные раскраски элементов
			for (size_t iData = 0; iData < frames[iID].frame.length; iData++)
			{
				// если новые данные такие же, что были ранее - необходимо плавно осветлять фоновую заливку
				if (frames[iID].frame.data[iData] == frame.data[iData])
				{
					if (frames[iID].color[iData] == themeColors.GridNewBackground)
					{
						frames[iID].color[iData] = themeColors.GridUpdateBackground;
						frames[iID].ms[iData] = ms;
					}
					else
					{
						// постепенно менять яркость для достижения фонового цвета по интервалу времени
						if (ms - frames[iID].ms[iData] > LIGTHNESS_INTERVAL)
						{
							if (themeColors.IsDark)
							{
								if (frames[iID].lightness[iData] > 0)
								{
									frames[iID].lightness[iData]--;
									frames[iID].ms[iData] = ms;
								}
							}
							else
							{
								if (frames[iID].lightness[iData] < 200)
								{
									frames[iID].lightness[iData]++;
									frames[iID].ms[iData] = ms;
								}
							}
						}
					}
				}
				else
				{
					frames[iID].color[iData] = themeColors.GridUpdateBackground;
					frames[iID].lightness[iData] = 100;
					frames[iID].ms[iData] = ms;
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
	std::fill_n(vFrame.color, 8, themeColors.GridNewBackground);
	int defaultLightness = 100;
	std::fill_n(vFrame.lightness, 8, defaultLightness);
	std::fill_n(vFrame.ms, 8, ms);
	frames.push_back(vFrame);
	std::sort(frames.begin(), frames.end());
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
