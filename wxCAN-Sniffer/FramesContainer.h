#pragma once

#include "Common.h"

#define FRAMES_DATA_RESERV	1000			// количество элементов для резерва в векторе
#define NEW_COLOR			0xFF00FF00lu	// (ABGR) зелёный - для выделения новых данных
#define CHANGE_COLOR		0xFF0000FFlu	// (ABGR) красный - для выделения изменяемых данных
#define DEFAULT_COLOR		0xFFFFFFFFlu	// (ABGR) белый   - цвет ячейки по умолчанию
#define SELECTED_COLOR		0xFFE3A04Clu	// (ABGR) голубой - цвет выделенных ячеек

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
	std::vector<VisualCANFrame> frames;		// хранилище пакетов с данными для визуализации
};
