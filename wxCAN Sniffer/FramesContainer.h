#pragma once
#include "Common.h"

#define FRAMES_DATA_RESERV	1000		// количество элементов дл€ резерва в векторе
#define NEW_COLOR			0xFF00FF00	// green (ABGR) - дл€ выделени€ новых данных
#define CHANGE_COLOR		0xFF0000FF	// red (ABGR)   - дл€ выделени€ измен€мых данных
#define DEFAULT_COLOR		0xFFFFFFFF	// white (ABGR) - цвет €чейки по умолчанию

class FramesContainer
{
public:
	FramesContainer();
	~FramesContainer();

	void Clear();
	void AddFrame(CANFrame& frame);
	size_t Size();
	VisualCANFrame GetFrame(size_t index);

private:
	vector<VisualCANFrame> frames;		// хранилище пакетов с данными дл€ визуализации
};
