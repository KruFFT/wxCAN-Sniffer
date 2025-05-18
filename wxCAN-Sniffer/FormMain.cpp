#include "FormMain.h"

// Обработка поступившего CAN-пакета
void FormMain::ProcessCANFrame(CANFrameIn& frame)
{
    bool found = false;

    // если это пакет с сервисным идентификатором - это статистика и её надо обработать отдельно
    if (frame.id == Parameters::can.ServiceID && frame.length >= 4)
    {
        uint16_t fps = *(uint16_t*)(&(frame.data[0]));
        textFPS->ChangeValue(wxString::Format(FORMAT_INT, fps));    // кадров в секунду
        uint16_t bps = *(uint16_t*)(&(frame.data[2]));
        textBPS->ChangeValue(wxString::Format(FORMAT_INT, bps));    // байтов в секунду
    }
    else
    {
        frames->AddFrame(frame);

        // добавить для отрисовки
        AddToDraw();

        // запись в лог по необходимости
        // если в фильтре ничего не выбрано - запись всех данных
        if (logFilterIDs.size() == 0)
        {
            SaveToLog(frame);
        }
        // иначе - записывать пакеты указанные в фильтре
        else
        {
            for (size_t iLog = 0; iLog < logFilterIDs.size(); iLog++)
            {
                if (frame.id == logFilterIDs[iLog])
                {
                    SaveToLog(frame);
                }
            }
        }

        // если ожидается ответ от этого ID - добавить его в список ответов
        if (frame.id == answerID)
        {
            int32_t lastRow = gridCANLog->GetNumberRows();
            gridCANLog->InsertRows(lastRow);

            if (frame.id & 0x80000000)
            {
                // расширенный пакет
                gridCANLog->SetCellValue(lastRow, 0, wxString::Format(FORMAT_HEX8, frame.id & 0x7FFFFFFF));
            }
            else
            {
                gridCANLog->SetCellValue(lastRow, 0, wxString::Format(FORMAT_HEX3, frame.id));
            }
            gridCANLog->SetCellValue(lastRow, 1, wxString::Format(FORMAT_INT, frame.length));

            // заполнение столбцов данных пакета
            for (size_t iData = 0; iData < 8; iData++)
            {
                if (iData < frame.length)
                {
                    // вывод данных
                    gridCANLog->SetCellValue(lastRow, 2 + iData, wxString::Format(FORMAT_HEX2, frame.data[iData]));
                }
            }
            // прокрутить список вниз
            gridCANLog->MakeCellVisible(gridCANLog->GetNumberRows() - 1, 0);
        }
    }
}

// Обновляет список ID для записи в log
void FormMain::RefreshListLog()
{
    sort(logFilterIDs.begin(), logFilterIDs.end());
    listLog->Clear();
    for (size_t iID = 0; iID < logFilterIDs.size(); iID++)
    {
        listLog->Append(wxString::Format(FORMAT_HEX3, logFilterIDs[iID]));
    }
}

// Записать в log указанный CAN-пакет
void FormMain::SaveToLog(CANFrameIn& frame)
{
    if (logEnable)
    {
        // если запись в один файл...
        if (logSingle)
        {
            // если файл существует - записать данные
            if (logFile)
            {
                LogWriteLine(logFile, frame);
            }
            else
            {
                // сначала создать файл
                try
                {
                    auto logPath = wxGetCwd() + wxFILE_SEP_PATH + TEXT_LOG_FILE_PREFIX + logExt;
                    logFile = new wxFFile();
                    if (logFile->Open(logPath, TEXT_LOG_FILE_ACCESS))
                    {
                        logFile->SeekEnd();
                        LogWriteLine(logFile, frame);
                    }
                }
                catch (...) {}
            }
        }
        else
        {
            // запись в разные файлы
            bool found = false;

            for (size_t iFile = 0; iFile < logFiles.size(); iFile++)
            {
                // проверка существования потока
                if (logFiles[iFile].id == frame.id)
                {
                    // если файл для этого кадра уже существует - просто дописать в него данные
                    LogWriteLine(logFiles[iFile].file, frame);

                    found = true;
                    break;
                }
            }

            // если файл не существует - сначала создать его, а потом записать строку
            if (!found)
            {
                try
                {
                    auto logPath = wxGetCwd() + TEXT_LOG_FILE_PREFIX_ID + wxString::Format(FORMAT_HEX3, frame.id) + logExt;

                    LogFile newLogFile = { 0 };
                    newLogFile.file = new wxFFile();
                    if (newLogFile.file->Open(logPath, TEXT_LOG_FILE_ACCESS))
                    {
                        newLogFile.file->SeekEnd();
                        newLogFile.id = frame.id;
                        logFiles.push_back(newLogFile);
                        LogWriteLine(logFiles[logFiles.size() - 1].file, frame);
                    }
                }
                catch (...) {}
            }
        }
    }
}

// Записать в журнал строку данных
void FormMain::LogWriteLine(wxFFile* file, CANFrameIn& frame)
{
    // штамп времени
    auto dtNow = wxDateTime::UNow();
    auto newLine = dtNow.FormatTime() + decimalSeparator + wxString::Format(FORMAT_INT3, dtNow.GetMillisecond()) + logSeparator;

    try
    {
        // идентификатор пакета и его длина
        newLine += wxString::Format(FORMAT_HEX3, frame.id) + logSeparator;
        newLine += wxString::Format(FORMAT_INT, frame.length) + logSeparator;
        // данные пакета
        for (size_t iData = 0; iData < frame.length; iData++)
        {
            if (logDecimal)
            {
                // десятичный вывод
                newLine += wxString::Format(FORMAT_INT0, frame.data[iData]) + logSeparator;
            }
            else
            {
                // шестнадцатеричный вывод
                newLine += wxString::Format(FORMAT_HEX2, frame.data[iData]) + logSeparator;
            }
        }

        // если надо сохранять ASCII данные
        std::string buf;
        if (logASCII)
        {
            // дополнить строку разделителями для выравнивания
            for (size_t iData = frame.length; iData < 8; iData++)
            {
                newLine += logSeparator;
            }
            newLine += TEXT_LOG_FILE_ASCII_SEPARATOR + logSeparator;

            // добавить ASCII данные из пакета
            buf = newLine.ToStdString();
            for (size_t iData = 0; iData < frame.length; iData++)
            {
                if (frame.data[iData] > 0x1F)
                {
                    buf += frame.data[iData];
                }
                else
                {
                    buf += ' ';
                }
            }
        }
        else
        {
            // данные без ASCII
            buf = newLine.ToStdString();
        }

        buf += "\n";
        file->Write(buf.c_str(), buf.size());
        file->Flush();
    }
    catch (...) {}
}

// Сбрасывает на диск данные и закрывает все файлы log-ов
void FormMain::FlushLogs()
{
    // сбросить данные на диск для массива логов
    for (size_t iFile = 0; iFile < logFiles.size(); iFile++)
    {
        logFiles[iFile].file->Flush();
        logFiles[iFile].file->Close();
        delete logFiles[iFile].file;
        logFiles[iFile].file = nullptr;
    }

    // сохранить всё, если лог один
    if (logFile)
    {
        logFile->Flush();
        logFile->Close();
        delete logFile;
        logFile = nullptr;
    }
}

// Преобразовывает байт в двоичное представление
wxString FormMain::ToBinary(uint8_t value)
{
    wxString binaryString;

    for (size_t counter = 0; counter < 4; counter++)
    {
        binaryString += (value & 0x80) ? TEXT_1 : TEXT_0;
        value <<= 1;
    }
    binaryString += TEXT_NUMBER_BLOCK_SEPARATOR;
    for (size_t counter = 0; counter < 4; counter++)
    {
        binaryString += (value & 0x80) ? TEXT_1 : TEXT_0;
        value <<= 1;
    }
    return binaryString;
}

// Преобразовывает 16-бит слово в двоичное представление
wxString FormMain::ToBinary(uint16_t value)
{
    wxString binaryString;

    for (size_t counter = 0; counter < 8; counter++)
    {
        binaryString += (value & 0x8000) ? TEXT_1 : TEXT_0;
        value <<= 1;
    }
    binaryString += TEXT_NUMBER_BLOCK_SEPARATOR;
    for (size_t counter = 0; counter < 8; counter++)
    {
        binaryString += (value & 0x8000) ? TEXT_1 : TEXT_0;
        value <<= 1;
    }
    return binaryString;
}

// Преобразовывает 32-бит слово в двоичное представление
wxString FormMain::ToBinary(uint32_t value)
{
    wxString binaryString;

    for (size_t counter = 0; counter < 8; counter++)
    {
        binaryString += (value & 0x80000000) ? TEXT_1 : TEXT_0;
        value <<= 1;
    }
    binaryString += TEXT_NUMBER_BLOCK_SEPARATOR;
    for (size_t counter = 0; counter < 8; counter++)
    {
        binaryString += (value & 0x80000000) ? TEXT_1 : TEXT_0;
        value <<= 1;
    }
    binaryString += TEXT_NUMBER_BLOCK_SEPARATOR;
    for (size_t counter = 0; counter < 8; counter++)
    {
        binaryString += (value & 0x80000000) ? TEXT_1 : TEXT_0;
        value <<= 1;
    }
    binaryString += TEXT_NUMBER_BLOCK_SEPARATOR;
    for (size_t counter = 0; counter < 8; counter++)
    {
        binaryString += (value & 0x80000000) ? TEXT_1 : TEXT_0;
        value <<= 1;
    }
    return binaryString;
}

// Обновить данные CAN-пакетов в таблице, вызывается по таймеру
void FormMain::RefreshGridCANView()
{
    size_t framesAmount = frames->Size();

    // заполнить таблицу строками
    while (gridCANView->GetNumberRows() < framesAmount)
    {
        gridCANView->InsertRows(0);
    }
    if (framesAmount > 0)
    {
        for (size_t iFrame = 0; iFrame < framesAmount; iFrame++)
        {
            // вывод ID, интервала и длины пакета
            auto vFrame = frames->GetFrame(iFrame);
            if (vFrame.frame.id & 0x80000000)
            {
                // расширенный пакет
                gridCANView->SetCellValue(iFrame, 0, wxString::Format(FORMAT_HEX8, vFrame.frame.id & 0x7FFFFFFF));
            }
            else
            {
                gridCANView->SetCellValue(iFrame, 0, wxString::Format(FORMAT_HEX3, vFrame.frame.id));
            }
            gridCANView->SetCellValue(iFrame, 1, wxString::Format(FORMAT_INT, vFrame.frame.interval));
            gridCANView->SetCellValue(iFrame, 2, wxString::Format(FORMAT_INT, vFrame.frame.length));
            if (Parameters::appearance.ControlsCustomColors)
            {
                gridCANView->SetCellBackgroundColour(iFrame, 0, Parameters::colors.GridBackground);
                gridCANView->SetCellBackgroundColour(iFrame, 1, Parameters::colors.GridBackground);
                gridCANView->SetCellBackgroundColour(iFrame, 2, Parameters::colors.GridBackground);
            }
            gridCANView->SetCellTextColour(iFrame, 0, Parameters::colors.GridText);
            gridCANView->SetCellTextColour(iFrame, 1, Parameters::colors.GridText);
            gridCANView->SetCellTextColour(iFrame, 2, Parameters::colors.GridText);

            // заполнение столбцов данных
            for (size_t iData = 0; iData < 8; iData++)
            {
                if (iData < vFrame.frame.length)
                {
                    // вывод данных с их фоновым цветом
                    gridCANView->SetCellValue(iFrame, iData + 3, wxString::Format(FORMAT_HEX2, vFrame.frame.data[iData]));
                    gridCANView->SetCellTextColour(iFrame, iData + 3, Parameters::colors.GridText);

                    uint8_t shift = vFrame.ticks[iData];
                    if (shift == TIMEOUT_PACKET)
                    {
                        gridCANView->SetCellBackgroundColour(iFrame, iData + 3, Parameters::colors.GridBackground);
                    }
                    else if (shift == NEW_PACKET)
                    {
                        gridCANView->SetCellBackgroundColour(iFrame, iData + 3, Parameters::colors.GridNewBackground);
                    }
                    else
                    {
                        auto backGroundColor = Parameters::colors.GridBackground;
                        auto color = vFrame.color[iData];

                        uint8_t rBackground = backGroundColor.GetRed();
                        uint8_t gBackground = backGroundColor.GetGreen();
                        uint8_t bBackground = backGroundColor.GetBlue();

                        uint8_t rColor = color.GetRed();
                        uint8_t gColor = color.GetGreen();
                        uint8_t bColor = color.GetBlue();

                        wxColour tunedColor;
                        if (Parameters::appearance.isDark)
                        {
                            uint8_t rDelta = shift * (rColor - rBackground) / PACKET_DELTA;
                            uint8_t gDelta = shift * (gColor - gBackground) / PACKET_DELTA;
                            uint8_t bDelta = shift * (bColor - bBackground) / PACKET_DELTA;
                            tunedColor = wxColour(rColor - rDelta, gColor - gDelta, bColor - bDelta);
                        }
                        else
                        {
                            uint8_t rDelta = shift * (rBackground - rColor) / PACKET_DELTA;
                            uint8_t gDelta = shift * (gBackground - gColor) / PACKET_DELTA;
                            uint8_t bDelta = shift * (bBackground - bColor) / PACKET_DELTA;
                            tunedColor = wxColour(rColor + rDelta, gColor + gDelta, bColor + bDelta);
                        }
                        gridCANView->SetCellBackgroundColour(iFrame, iData + 3, tunedColor);
                    }
                }
                else
                {
                    // вывод пустых ячеек
                    gridCANView->SetCellValue(iFrame, iData + 3, TEXT_SPACE);
                    gridCANView->SetCellTextColour(iFrame, iData + 3, Parameters::colors.GridText);
                    gridCANView->SetCellBackgroundColour(iFrame, iData + 3, Parameters::colors.GridBackground);
                }
            }
        }
        // раскраска выделенных ячеек
        if (colToView >= 0)
        {
            wxColour backgroundColor = Parameters::appearance.ControlsCustomColors ? Parameters::colors.GridSelectedBackground : gridSystemSelectedBackgroundColor;
            gridCANView->SetCellBackgroundColour(rowToView, colToView + 3, backgroundColor);
            switch (dataType)
            {
                case DataTypes::UInt16:
                case DataTypes::Int16:
                    if (colToView < 7)
                    {
                        gridCANView->SetCellBackgroundColour(rowToView, colToView + 4, backgroundColor);
                    }
                    break;

                case DataTypes::UInt32:
                case DataTypes::Int32:
                case DataTypes::Float:
                    if (colToView < 5)
                    {
                        gridCANView->SetCellBackgroundColour(rowToView, colToView + 4, backgroundColor);
                        gridCANView->SetCellBackgroundColour(rowToView, colToView + 5, backgroundColor);
                        gridCANView->SetCellBackgroundColour(rowToView, colToView + 6, backgroundColor);
                    }
                    else if (colToView < 6)
                    {
                        gridCANView->SetCellBackgroundColour(rowToView, colToView + 4, backgroundColor);
                        gridCANView->SetCellBackgroundColour(rowToView, colToView + 5, backgroundColor);
                    }
                    else if (colToView < 7)
                    {
                        gridCANView->SetCellBackgroundColour(rowToView, colToView + 4, backgroundColor);
                    }
                    break;
            }
        }
        // обновить отображение таблицы
        gridCANView->RefreshBlock(0, 0, framesAmount - 1, 10);
    }
}

// Отображение выделенной ячейки в разных форматах
void FormMain::ShowNumbers()
{
    if (rowToView >= 0 && colToView >= 0)
    {
        // если полученных данных ещё нет
        if (rowToView >= frames->Size())
        {
            return;
        }

        auto vFrame = frames->GetFrame(rowToView);

        switch (dataType)
        {
            case DataTypes::UInt8:
            {
                uint8_t byte1 = vFrame.frame.data[colToView];
                textBin->ChangeValue(ToBinary(byte1));
                textDec->ChangeValue(wxString::Format(FORMAT_UINT, byte1));
                uint32_t mulValue = (float)byte1 * mul;
                textDecimalResult->ChangeValue(wxString::Format(FORMAT_UINT, mulValue));
                break;
            }

            case DataTypes::UInt16:
            {
                uint8_t byte1 = vFrame.frame.data[colToView];
                uint8_t byte2 = colToView < 7 ? vFrame.frame.data[colToView + 1] : 0;
                // выбор между big endian и little endian
                uint16_t value = bigEndian ? (byte1 << 8) | byte2 : (byte2 << 8) | byte1;
                textBin->ChangeValue(ToBinary((uint16_t)value));
                textDec->ChangeValue(wxString::Format(FORMAT_UINT, value));
                uint32_t mulValue = (float)value * mul;
                textDecimalResult->ChangeValue(wxString::Format(FORMAT_UINT, mulValue));
                break;
            }

            case DataTypes::UInt32:
            {
                uint8_t byte1 = vFrame.frame.data[colToView];
                uint8_t byte2 = colToView < 7 ? vFrame.frame.data[colToView + 1] : 0;
                uint8_t byte3 = colToView < 6 ? vFrame.frame.data[colToView + 2] : 0;
                uint8_t byte4 = colToView < 5 ? vFrame.frame.data[colToView + 3] : 0;
                // выбор между big endian и little endian
                uint32_t value = bigEndian ? (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4 : (byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1;
                textBin->ChangeValue(ToBinary(value));
                textDec->ChangeValue(wxString::Format(FORMAT_UINT, value));
                uint32_t mulValue = (float)value * mul;
                textDecimalResult->ChangeValue(wxString::Format(FORMAT_UINT, mulValue));
                break;
            }

            case DataTypes::Int8:
            {
                uint8_t byte1 = vFrame.frame.data[colToView];
                textBin->ChangeValue(ToBinary(byte1));
                textDec->ChangeValue(wxString::Format(FORMAT_INT, (int8_t)byte1));
                int32_t mulValue = (float)((int8_t)byte1) * mul;
                textDecimalResult->ChangeValue(wxString::Format(FORMAT_INT, mulValue));
                break;
            }

            case DataTypes::Int16:
            {
                uint8_t byte1 = vFrame.frame.data[colToView];
                uint8_t byte2 = colToView < 7 ? vFrame.frame.data[colToView + 1] : 0;
                // выбор между big endian и little endian
                uint16_t value = bigEndian ? (byte1 << 8) | byte2 : (byte2 << 8) | byte1;
                textBin->ChangeValue(ToBinary((uint16_t)value));
                textDec->ChangeValue(wxString::Format(FORMAT_INT, (int16_t)value));
                int32_t mulValue = (float)((int16_t)value) * mul;
                textDecimalResult->ChangeValue(wxString::Format(FORMAT_INT, mulValue));
                break;
            }

            case DataTypes::Int32:
            {
                uint8_t byte1 = vFrame.frame.data[colToView];
                uint8_t byte2 = colToView < 7 ? vFrame.frame.data[colToView + 1] : 0;
                uint8_t byte3 = colToView < 6 ? vFrame.frame.data[colToView + 2] : 0;
                uint8_t byte4 = colToView < 5 ? vFrame.frame.data[colToView + 3] : 0;
                // выбор между big endian и little endian
                uint32_t value = bigEndian ? (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4 : (byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1;
                textBin->ChangeValue(ToBinary(value));
                textDec->ChangeValue(wxString::Format(FORMAT_INT, (int32_t)value));
                int32_t mulValue = (float)((int32_t)value) * mul;
                textDecimalResult->ChangeValue(wxString::Format(FORMAT_INT, mulValue));
                break;
            }

            case DataTypes::Float:
            {
                uint8_t byte1 = vFrame.frame.data[colToView];
                uint8_t byte2 = colToView < 7 ? vFrame.frame.data[colToView + 1] : 0;
                uint8_t byte3 = colToView < 6 ? vFrame.frame.data[colToView + 2] : 0;
                uint8_t byte4 = colToView < 5 ? vFrame.frame.data[colToView + 3] : 0;
                // выбор между big endian и little endian
                uint32_t value = bigEndian ? (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4 : (byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1;
                textBin->ChangeValue(ToBinary(value));
                auto floatValue = *(float*)((uint32_t*)&value);
                textDec->ChangeValue(wxString::Format(FORMAT_FLOAT1_3, floatValue));
                float mulValue = floatValue * mul;
                textDecimalResult->ChangeValue(wxString::Format(FORMAT_FLOAT1_3, mulValue));
                break;
            }
        }
    }
}

// Добавление полученных данных в очередь на отрисовку
void FormMain::AddToDraw()
{
    if (rowToView >= 0 && colToView >= 0)
    {
        // если полученных данных ещё нет
        if (rowToView >= frames->Size())
        {
            return;
        }

        // добавить полученные данные в очередь на отрисовку
        if (drawData && colToView >= 0)
        {
            auto vFrame = frames->GetFrame(rowToView);

            switch (dataType)
            {
                case DataTypes::UInt8:
                {
                    uint8_t byte1 = vFrame.frame.data[colToView];
                    float mulValue = (float)byte1 * mul;
                    drawData->Add(mulValue);
                    break;
                }

                case DataTypes::UInt16:
                {
                    uint8_t byte1 = vFrame.frame.data[colToView];
                    uint8_t byte2 = colToView < 7 ? vFrame.frame.data[colToView + 1] : 0;
                    // выбор между big endian и little endian
                    uint16_t value = bigEndian ? (byte1 << 8) | byte2 : (byte2 << 8) | byte1;
                    float mulValue = (float)value * mul;
                    drawData->Add(mulValue);
                    break;
                }

                case DataTypes::UInt32:
                {
                    uint8_t byte1 = vFrame.frame.data[colToView];
                    uint8_t byte2 = colToView < 7 ? vFrame.frame.data[colToView + 1] : 0;
                    uint8_t byte3 = colToView < 6 ? vFrame.frame.data[colToView + 2] : 0;
                    uint8_t byte4 = colToView < 5 ? vFrame.frame.data[colToView + 3] : 0;
                    // выбор между big endian и little endian
                    uint32_t value = bigEndian ? (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4 : (byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1;
                    float mulValue = (float)value * mul;
                    drawData->Add(mulValue);
                    break;
                }

                case DataTypes::Int8:
                {
                    uint8_t byte1 = vFrame.frame.data[colToView];
                    float mulValue = (float)((int8_t)byte1) * mul;
                    drawData->Add(mulValue);
                    break;
                }

                case DataTypes::Int16:
                {
                    uint8_t byte1 = vFrame.frame.data[colToView];
                    uint8_t byte2 = colToView < 7 ? vFrame.frame.data[colToView + 1] : 0;
                    // выбор между big endian и little endian
                    uint16_t value = bigEndian ? (byte1 << 8) | byte2 : (byte2 << 8) | byte1;
                    float mulValue = (float)((int16_t)value) * mul;
                    drawData->Add(mulValue);
                    break;
                }

                case DataTypes::Int32:
                {
                    uint8_t byte1 = vFrame.frame.data[colToView];
                    uint8_t byte2 = colToView < 7 ? vFrame.frame.data[colToView + 1] : 0;
                    uint8_t byte3 = colToView < 6 ? vFrame.frame.data[colToView + 2] : 0;
                    uint8_t byte4 = colToView < 5 ? vFrame.frame.data[colToView + 3] : 0;
                    // выбор между big endian и little endian
                    uint32_t value = bigEndian ? (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4 : (byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1;
                    float mulValue = (float)((int32_t)value) * mul;
                    drawData->Add(mulValue);
                    break;
                }

                case DataTypes::Float:
                {
                    uint8_t byte1 = vFrame.frame.data[colToView];
                    uint8_t byte2 = colToView < 7 ? vFrame.frame.data[colToView + 1] : 0;
                    uint8_t byte3 = colToView < 6 ? vFrame.frame.data[colToView + 2] : 0;
                    uint8_t byte4 = colToView < 5 ? vFrame.frame.data[colToView + 3] : 0;
                    // выбор между big endian и little endian
                    uint32_t value = bigEndian ? (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4 : (byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1;
                    float mulValue = *(float*)((uint32_t*)&value) * mul;
                    drawData->Add(mulValue);
                    break;
                }
            }
        }
    }
}

// Отправить CAN-пакет через UDP-сокет
void FormMain::UDPSocket_SendFrame(CANFrameOut& frame)
{
    SendCANFrame sendCANFrame;
    sendCANFrame.Signature = Parameters::can.Signature;
    sendCANFrame.Frame = frame;
    udpSocket->SendTo(mcIpAddress, &sendCANFrame, sizeof(SendCANFrame));
}

// Отправить команду управления
void FormMain::SendCANCommand(CANCommands command, uint16_t speed)
{
    CANFrameOut frame = { 0 };
    switch (command)
    {
        case CANCommands::Disconnect:
            // собрать управляющий CAN-пакет отключения
            frame.id = Parameters::can.ServiceID;
            frame.length = 1;
            frame.data[0] = 0;
            break;

        case CANCommands::Connect:
            // собрать управляющий CAN-пакет подключения
            frame.id = Parameters::can.ServiceID;
            frame.length = 3;
            frame.data[0] = 1;
            *(uint16_t*)(&(frame.data[1])) = speed;
            break;

        default:
            return;
            break;
    }

    switch (mode)
    {
        case Modes::Serial:
            if (serialPort)
            {
                serialPort->SendFrame(frame);
            }
            break;

        case Modes::Network:
            if (udpSocket)
            {
                UDPSocket_SendFrame(frame);
            }
            break;
    }
}

// Включить элементы управления соединением
void FormMain::EnableConnectionControls()
{
    choiceMode->Enable();
    comboBoxDataSource->Enable();
    choiceCANSpeed->Enable();
}

// Отключить элементы управления соединением
void FormMain::DisableConnectionControls()
{
    choiceMode->Disable();
    comboBoxDataSource->Disable();
    choiceCANSpeed->Disable();
}
