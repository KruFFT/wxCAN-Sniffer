#pragma once

#include "Common.h"

#define FRAMES_DATA_RESERV  1000            // количество элементов для резерва в векторе

class FramesContainer
{
public:
    FramesContainer();
    ~FramesContainer();

    void Clear();                           // очистить контейнер
    void AddFrame(CANFrameIn& frame);       // добавить пакет в контейнер
    void ProcessAllFrames();                // обработать все пакеты на "выцветание"
    size_t Size();
    VisualCANFrame GetFrame(size_t index);

private:
    std::vector<VisualCANFrame> frames;     // хранилище пакетов с данными для визуализации
};
