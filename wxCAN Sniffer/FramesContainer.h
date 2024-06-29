#pragma once
#include "Common.h"

#define FRAMES_DATA_RESERV	1000		// количество элементов для резерва в векторе
#define NEW_COLOR			0xFF00FF00	// green (ABGR) - для выделения новых данных
#define CHANGE_COLOR		0xFF0000FF	// red (ABGR)   - для выделения изменямых данных
#define DEFAULT_COLOR		0xFFFFFFFF	// white (ABGR) - цвет ячейки по умолчанию

class FramesContainer
{
public:
	FramesContainer();
	~FramesContainer();

	void Clear();
	void AddFrame(CANFrameIn& frame);
	size_t Size();
	VisualCANFrame GetFrame(size_t index);

private:
	vector<VisualCANFrame> frames;		// хранилище пакетов с данными для визуализации
};
