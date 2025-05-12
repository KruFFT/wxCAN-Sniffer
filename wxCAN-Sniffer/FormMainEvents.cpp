#include "FormMain.h"

// Таблица событий
wxBEGIN_EVENT_TABLE(FormMain, wxFrame)
wxEND_EVENT_TABLE()

// События из фонового потока последовательного порта
wxDEFINE_EVENT(wxEVT_SERIAL_PORT_THREAD_STARTED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_SERIAL_PORT_THREAD_UPDATE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_SERIAL_PORT_THREAD_EXIT, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_SERIAL_PORT_THREAD_MESSAGE, wxThreadEvent);

// Обработчики событий главного окна
void FormMain::AssignEventHandlers()
{
    // привязка событий
    this->Bind(wxEVT_CLOSE_WINDOW, &FormMain::OnClose, this, IDs::MAIN_FORM);
    choiceMode->Bind(wxEVT_CHOICE, &FormMain::ChoiceMode_OnChoice, this);
    choiceCANSpeed->Bind(wxEVT_CHOICE, &FormMain::ChoiceCANSpeed_OnChoice, this);
    buttonConnectDisconnect->Bind(wxEVT_BUTTON, &FormMain::ButtonConnectDisconnect_OnClick, this);
    buttonAdd->Bind(wxEVT_BUTTON, &FormMain::ButtonAdd_OnClick, this);
    buttonRemove->Bind(wxEVT_BUTTON, &FormMain::ButtonRemove_OnClick, this);
    buttonRemoveAll->Bind(wxEVT_BUTTON, &FormMain::ButtonRemoveAll_OnClick, this);
    buttonSend->Bind(wxEVT_BUTTON, &FormMain::ButtonSend_OnClick, this);
    buttonClearCANLog->Bind(wxEVT_BUTTON, &FormMain::ButtonClearCANLog_OnClick, this);
    gridCANView->Bind(wxEVT_GRID_SELECT_CELL, &FormMain::GridCANView_OnSelectCell, this);
    checkLogEnable->Bind(wxEVT_CHECKBOX, &FormMain::CheckLogEnable_OnClick, this);
    choiceExt->Bind(wxEVT_CHOICE, &FormMain::ChoiceExt_OnChoice, this);
    choiceSep->Bind(wxEVT_CHOICE, &FormMain::ChoiceSep_OnChoice, this);
    checkDec->Bind(wxEVT_CHECKBOX, &FormMain::CheckDec_OnClick, this);
    checkSingle->Bind(wxEVT_CHECKBOX, &FormMain::CheckSingle_OnClick, this);
    choiceDataType->Bind(wxEVT_CHOICE, &FormMain::ChoiceDataType_OnChoice, this);
    checkEndian->Bind(wxEVT_CHECKBOX, &FormMain::CheckEndian_OnClick, this);
    checkASCII->Bind(wxEVT_CHECKBOX, &FormMain::CheckASCII_OnClick, this);
    textDecWordMul->Bind(wxEVT_TEXT_ENTER, &FormMain::TextDecWordMul_OnEnter, this);
    textCANAnswerID->Bind(wxEVT_TEXT_ENTER, &FormMain::TextCANAnswerID_OnEnter, this);

    // события панели графика
    drawPanel->Bind(wxEVT_PAINT, &FormMain::DrawPanel_OnPaint, this);
    drawPanel->Bind(wxEVT_ERASE_BACKGROUND, &FormMain::DrawPanel_OnEraseBackground, this);

    // событие от фонового потока COM-порта
    this->Bind(wxEVT_SERIAL_PORT_THREAD_STARTED, &FormMain::Thread_OnStarted, this);
    this->Bind(wxEVT_SERIAL_PORT_THREAD_UPDATE, &FormMain::Thread_OnUpdate, this);
    this->Bind(wxEVT_SERIAL_PORT_THREAD_EXIT, &FormMain::Thread_OnExit, this);
    this->Bind(wxEVT_SERIAL_PORT_THREAD_MESSAGE, &FormMain::Thread_OnMessage, this);

    // событие данных из UDP-сокета
    this->Bind(wxEVT_SOCKET, &FormMain::UDPSocket_OnEvent, this, IDs::UDP_SOCKET);

    // событие таймера
    this->Bind(wxEVT_TIMER, &FormMain::MainTimer_OnTimer, this, IDs::MAIN_TIMER);
}

// Срабатывание главного таймера - обработка данных
void FormMain::MainTimer_OnTimer(wxTimerEvent& event)
{
    // проверка состояния последовательного порта
    if (mode == Modes::Serial && serialPort && serialPort->hSerial == INVALID_HANDLE_VALUE)
    {
        delete serialPort;
        serialPort = nullptr;
        buttonConnectDisconnect->SetLabelText(TEXT_CONNECT);
        EnableConnectionControls();
    }

    // обработка всех пакетов на "выцветание"
    frames->ProcessAllFrames();

    if (screenUpdateCounter >= SCREEN_UPDATE_COUNTER_LIMIT)
    {
        // обновить данные в таблице
        RefreshGridCANView();

        ShowNumbers();

        // это вызовет событие OnPaint для панели
        drawPanel->Refresh();
        screenUpdateCounter = 0;
    }

    screenUpdateCounter++;
}

// Выбор режима работы
void FormMain::ChoiceMode_OnChoice(wxCommandEvent& event)
{
    mode = (Modes)event.GetSelection();
    switch (mode)
    {
        case Modes::Serial:
            comboBoxSerialPort->Show(true);
            comboBoxIPAddress->Show(false);
            break;

        case Modes::Network:
            comboBoxIPAddress->Show(true);
            comboBoxSerialPort->Show(false);
            break;
    }
    Layout();
}

// Выбор скорости CAN-шины
void FormMain::ChoiceCANSpeed_OnChoice(wxCommandEvent& event)
{
    uint32_t value;
    if (event.GetString().ToUInt(&value))
    {
        selectedCanSpeed = (uint16_t)value;
    }
    else
    {
        selectedCanSpeed = 500;
    }
}

// Обработчик кнопки Подключить/отключить
void FormMain::ButtonConnectDisconnect_OnClick(wxCommandEvent& event)
{
    rowToView = -1;
    colToView = -1;
    drawData->Clear();

    switch (mode)
    {
        // режим: последовательный порт
        case Modes::Serial:
            try
            {
                // если порт не открыт - открыть, иначе - закрыть
                if (serialPort == nullptr)
                {
                    DisableConnectionControls();
                    frames->Clear();

                    // удалить все строки таблицы
                    if (gridCANView->GetNumberRows() > 0)
                    {
                        gridCANView->DeleteRows(0, gridCANView->GetNumberRows());
                    }

                    serialPort = new ThreadedSerialPort(comboBoxSerialPort->GetValue(), Parameters::serial.PortSpeed, (wxFrame*)this);
                    // поток стартует с задержкой, все остальные проверки его состояния в событии таймера MainTimer_OnTimer
                    buttonConnectDisconnect->SetLabelText(TEXT_DISCONNECT);
                    // команда подключения будет отправлена в событии запуска потока Thread_OnStarted
                }
                else
                {
                    // отправить управляющий пакет отключения
                    SendCANCommand(CANCommands::Disconnect);
                    // просто запустить процесс остановки потока, все остальные проверки его состояния в событии таймера MainTimer_OnTimer
                    serialPort->Delete();
                    // записать все log-файлы
                    FlushLogs();
                    logFiles.clear();
                    EnableConnectionControls();
                }
            }
            catch (...)
            {
                buttonConnectDisconnect->SetLabelText(TEXT_CONNECT);
                EnableConnectionControls();
                wxMessageBox(ERROR_SERIAL);
            }
            break;
            
        // режим: сеть
        case Modes::Network:
            try
            {
                // отправка команды подключения/отключения, если есть сетевое соединение
                if (udpSocket)
                {
                    if (!udpConnected)
                    {
                        SendCANCommand(CANCommands::Connect, selectedCanSpeed);
                        buttonConnectDisconnect->SetLabelText(TEXT_DISCONNECT);
                        DisableConnectionControls();
                    }
                    else
                    {
                        SendCANCommand(CANCommands::Disconnect);
                        buttonConnectDisconnect->SetLabelText(TEXT_CONNECT);
                        EnableConnectionControls();
                    }
                    udpConnected = !udpConnected;
                }
            }
            catch (...)
            {
                buttonConnectDisconnect->SetLabelText(TEXT_CONNECT);
                EnableConnectionControls();
            }
            break;
    }
}

// Поток работы с последовательным портом запустился
void FormMain::Thread_OnStarted(wxThreadEvent& event)
{
    // отправить управляющий пакет подключения
    SendCANCommand(CANCommands::Connect, selectedCanSpeed);
}

// По событию от потока забирать все принятые CAN-пакеты, которые есть в буфере
void FormMain::Thread_OnUpdate(wxThreadEvent& event)
{
    CANFrameIn frame;

    if (serialPort)
    {
        while (serialPort->GetNextFrame(frame))
        {
            ProcessCANFrame(frame);
        }
    }
}

// По событию от потока забирать все принятые CAN-пакеты, которые есть в буфере
void FormMain::Thread_OnExit(wxThreadEvent& event)
{
    if (serialPort)
    {
        serialPort = nullptr;
    }
    buttonConnectDisconnect->SetLabelText(TEXT_CONNECT);
    EnableConnectionControls();
}

// Сообщение от потока при ошибке
void FormMain::Thread_OnMessage(wxThreadEvent& event)
{
    wxMessageBox(event.GetPayload<wxString>(), ERROR_CAPTION, wxICON_ERROR);
}

// Выбран другой тип данных для отображения
void FormMain::ChoiceDataType_OnChoice(wxCommandEvent& event)
{
    dataType = (DataTypes)event.GetSelection();
    checkEndian->Enable(dataType != DataTypes::UInt8 && dataType != DataTypes::Int8);
}

// При нажатии Enter в поле ввода множителя - обработать его и запомнить
void FormMain::TextDecWordMul_OnEnter(wxCommandEvent& event)
{
    double newMul;
    if (textDecWordMul->GetValue().ToDouble(&newMul))
    {
        float newFloatMul = (float)newMul;
        if (newFloatMul == 0)
            mul = 1.0;
        else if (newFloatMul > 10000)
            mul = 10000.0;
        else
            mul = newFloatMul;
    }

    textDecWordMul->ChangeValue(wxString::Format(FORMAT_FLOAT1_3, mul));
}

// Добавить ID в список фильтра для записи в log
void FormMain::ButtonAdd_OnClick(wxCommandEvent& event)
{
    if (rowToLog >= 0 && (int)frames->Size() > rowToLog)
    {
        bool found = false;
        for (size_t iID = 0; iID < logFilterIDs.size(); iID++)
        {
            if (frames->GetFrame(rowToLog).frame.id == logFilterIDs[iID])
            {
                found = true;
                break;
            }
        }
        if (!found)
            logFilterIDs.push_back(frames->GetFrame(rowToLog).frame.id);
    }

    RefreshListLog();
}

// Удалить ID из списка фильтра записи в log
void FormMain::ButtonRemove_OnClick(wxCommandEvent& event)
{
    int32_t index = listLog->GetSelection();

    if (index > -1)
    {
        logFilterIDs.erase(logFilterIDs.begin() + index);
    }

    RefreshListLog();
}

// Очистить весь список фильтра записи в log
void FormMain::ButtonRemoveAll_OnClick(wxCommandEvent& event)
{
    logFilterIDs.clear();
    RefreshListLog();
}

// Выбор разрешения записи в журнал
void FormMain::CheckLogEnable_OnClick(wxCommandEvent& event)
{
    logEnable = event.IsChecked();
}

// Выбор расширения файла
void FormMain::ChoiceExt_OnChoice(wxCommandEvent& event)
{
    logExt = event.GetString();
}

// Выбор разделителя в log-файле
void FormMain::ChoiceSep_OnChoice(wxCommandEvent& event)
{
    auto value = event.GetString();
    if (value == TEXT_LOG_SEPARATOR_TAB)
        logSeparator = TEXT_LOG_SEPARATOR_TAB_VALUE;
    else if (value == TEXT_LOG_SEPARATOR_SPACE)
        logSeparator = TEXT_LOG_SEPARATOR_SPACE_VALUE;
    else
        logSeparator = value;
}

// Клик по десятичной форме записи log
void FormMain::CheckDec_OnClick(wxCommandEvent& event)
{
    logDecimal = event.IsChecked();
}

// Клик по выводу в один файл
void FormMain::CheckSingle_OnClick(wxCommandEvent& event)
{
    logSingle = event.IsChecked();
}

// Клик для добавления в log-файл ASCII данных
void FormMain::CheckASCII_OnClick(wxCommandEvent& event)
{
    logASCII = event.IsChecked();
}

// Клик по выбору big endian или little endian
void FormMain::CheckEndian_OnClick(wxCommandEvent& event)
{
    bigEndian = event.IsChecked();
}

// Выбор ячейки в таблице
void FormMain::GridCANView_OnSelectCell(wxGridEvent& event)
{
    // запомнить строку для добавления в log
    rowToLog = event.GetRow();

    rowToView = rowToLog;
    colToView = event.GetCol() - 3;
    // если выбран столбик не с данными
    if (colToView < 0)
    {
        rowToView = -1;
        colToView = -1;

        textBin->ChangeValue(wxEmptyString);
        textDec->ChangeValue(wxEmptyString);
        textDecimalResult->ChangeValue(wxEmptyString);
    }
    else
    {
        // показать числа в разных форматах
        ShowNumbers();
    }
}

// Нажатие кнопки очистки ответа на отправленный пакет
void FormMain::ButtonClearCANLog_OnClick(wxCommandEvent& event)
{
    // удалить все строки таблицы
    if (gridCANLog->GetNumberRows() > 0)
    {
        gridCANLog->DeleteRows(0, gridCANLog->GetNumberRows());
    }
}

// Нажатие кнопки отправки CAN-пакета
void FormMain::ButtonSend_OnClick(wxCommandEvent& event)
{
    unsigned long tempValue;

    // собрать CAN-пакет для отправки
    CANFrameOut frame = { 0 };

    // ID пакета
    textCANID->GetValue().ToULong(&tempValue, 16);
    if (tempValue > 0)
        frame.id = (uint32_t)tempValue;
    else
        return;
    // зарезервировано
    frame.reserved = 0;
    // длина данных пакета
    textCANLength->GetValue().ToULong(&tempValue, 10);
    if (tempValue >= 0 && tempValue <= 8)
        frame.length = (uint8_t)tempValue;
    else
        return;
    // байт 1
    textCANByte0->GetValue().ToULong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[0] = (uint8_t)tempValue;
    else
        return;
    // байт 2
    textCANByte1->GetValue().ToULong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[1] = (uint8_t)tempValue;
    else
        return;
    // байт 3
    textCANByte2->GetValue().ToULong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[2] = (uint8_t)tempValue;
    else
        return;
    // байт 4
    textCANByte3->GetValue().ToULong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[3] = (uint8_t)tempValue;
    else
        return;
    // байт 5
    textCANByte4->GetValue().ToULong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[4] = (uint8_t)tempValue;
    else
        return;
    // байт 6
    textCANByte5->GetValue().ToULong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[5] = (uint8_t)tempValue;
    else
        return;
    // байт 7
    textCANByte6->GetValue().ToULong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[6] = (uint8_t)tempValue;
    else
        return;
    // байт 8
    textCANByte7->GetValue().ToULong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[7] = (uint8_t)tempValue;
    else
        return;
    // запомнить ID пакета, от которого ожидается ответ
    textCANAnswerID->GetValue().ToULong(&tempValue, 16);
    if (tempValue > 0)
        answerID = (uint32_t)tempValue;
    else
        answerID = 0;

    // отправить данные
    if (serialPort)
    {
        serialPort->SendFrame(frame);
    }
    if (udpSocket)
    {
        UDPSocket_SendFrame(frame);
    }
}

// Нажатие Enter при вводе номера CAN ID ожидания ответа
void FormMain::TextCANAnswerID_OnEnter(wxCommandEvent& event)
{
    unsigned long value;
    textCANAnswerID->GetValue().ToCULong(&value, 16);
    answerID = (uint32_t)value;
    textCANAnswerID->ChangeValue(wxString::Format(FORMAT_HEX3, answerID));
}

// Событие UDP-сокета
void FormMain::UDPSocket_OnEvent(wxSocketEvent& event)
{
    uint8_t  receivedData[UDP_BUFFER_SIZE] = { 0 };
    uint8_t* receivedDataPointer = receivedData;

    if (event.GetSocketEvent() == wxSOCKET_INPUT)
    {
        size_t receivedDataLen = udpSocket->RecvFrom(mcIpAddress, receivedData, UDP_BUFFER_SIZE).LastCount();
        if (receivedDataLen)
        {
            CANFrameIn frame;
            uint8_t* receivedDataTail = receivedDataPointer + receivedDataLen;
            // поиск CAN-пакета и формирование данных
            while (receivedDataPointer < receivedDataTail)
            {
                // пакет собран - обработать пакет
                if (CANParser::Parse(&receivedDataPointer, frame))
                {
                    ProcessCANFrame(frame);
                }
            }
        }
    }
}

// Событие отрисовки в панели графика
void FormMain::DrawPanel_OnPaint(wxPaintEvent& event)
{
    //wxPaintDC dc(drawPanel);
    //wxBufferedPaintDC dc(drawPanel);
    wxAutoBufferedPaintDC dc(drawPanel);
    PrepareDC(dc);
    auto drawRectangle = drawPanel->GetClientRect();

    // нарисовать рамку и фон
    dc.SetPen(graphFramePen);
    dc.SetBrush(graphBackgroundBrush);
    dc.DrawRectangle(drawRectangle);

    if (drawData && colToView >= 0)
    {
        float* frame = drawData->Frame();

        float minValue = 0;
        float maxValue = 0;
        // поиск наибольшего значения для графика
        for (size_t index = 0; index < drawFrameSize; index++)
        {
            float nextValue = *(frame + index);
            if (nextValue < minValue) minValue = nextValue;
            if (nextValue > maxValue) maxValue = nextValue;
        }

        // вычисление масштабного коэффициента
        float height = (float)drawRectangle.height;
        float scaleFactor = -height / (maxValue - minValue);

        // нарисовать осевую линию по нулю
        dc.SetPen(Parameters::colors.GraphText);
        wxCoord y = (-minValue) * scaleFactor + height;
        dc.DrawLine(0, y, drawRectangle.width, y);

        // рисовать отмасштабированный график
        dc.SetPen(graphPen);
        y = (*frame - minValue) * scaleFactor + height;
        for (size_t index = 1; index < drawFrameSize; index++)
        {
            wxCoord yy = (*(frame + index) - minValue) * scaleFactor + height;
            dc.DrawLine(index - 1, y, index, yy);
            y = yy;
        }

        dc.SetTextForeground(Parameters::colors.GraphText);
        auto fontMetrics = dc.GetFontMetrics();
        dc.DrawText(wxString::Format(FORMAT_FLOAT1_0, maxValue), fontMetrics.internalLeading, 0);
        dc.DrawText(wxString::Format(FORMAT_FLOAT1_0, minValue), fontMetrics.internalLeading, drawRectangle.height - fontMetrics.height - fontMetrics.descent);
    }
}

// Стирание фона панели графика
void FormMain::DrawPanel_OnEraseBackground(wxEraseEvent& event)
{
    // кода нет, используется как заглушка
}
