#pragma once

#include "Common.h"

#define FRAMES_DATA_RESERV	1000			// количество элементов для резерва в векторе
#define LIGTHNESS_INTERVAL	20				// интервал изменения яркости цвета ячейки

class FramesContainer
{
public:
	FramesContainer(WindowColors colors);
	~FramesContainer();

	void Clear();
	void AddFrame(CANFrameIn& frame);
	size_t Size();
	VisualCANFrame GetFrame(size_t index);

private:
	std::vector<VisualCANFrame> frames;		// хранилище пакетов с данными для визуализации
	WindowColors themeColors;				// цвета объектов
};
