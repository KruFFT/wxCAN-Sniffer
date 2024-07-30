#include "FormMain.h"

// События из фонового потока
wxDEFINE_EVENT(wxEVT_SERIAL_PORT_THREAD_UPDATE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_SERIAL_PORT_THREAD_EXIT, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_SERIAL_PORT_THREAD_MESSAGE, wxThreadEvent);

// Таблица событий (только событие для UDP-сокета)
wxBEGIN_EVENT_TABLE(FormMain, wxFrame)
    EVT_SOCKET(ID_UDP_SOCKET, FormMain::UDPSocket_OnEvent)
wxEND_EVENT_TABLE()

// Конструктор окна
FormMain::FormMain(WindowColors& colors) : wxFrame(nullptr, ID_MAIN_FORM, CAPTION, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE)
{
    // сохранение цветов
    themeColors = colors;
    graphFramePen = wxPen(themeColors.GraphFrame, 1);
    graphBackgroundBrush = wxBrush(themeColors.GraphBackground);
    graphPen = wxPen(themeColors.GraphDraw, 3);
    graphText = themeColors.GraphText;

    // иконка и размер окна
    auto icon = wxIcon(canIcon);
    this->SetIcon(icon);
    this->SetSizeHints(FromDIP(wxSize(1240, 600)));

    // главный сайзер окна
    auto sizerMain = new wxBoxSizer(wxHORIZONTAL);
    {
        splitterLeft = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE | wxSP_3D);

        panelLeftTop = new wxPanel(splitterLeft, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
        panelLeftBottom = new wxPanel(splitterLeft, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);

        // левый верхний сайзер
        auto sizerLeftTop = new wxStaticBoxSizer(new wxStaticBox(panelLeftTop, wxID_ANY, wxT("Просмотр пакетов")), wxVERTICAL);
        {
            gridCANView = new wxGrid(panelLeftTop, wxID_ANY);
            // параметры сетки
            gridCANView->CreateGrid(0, 11);
            gridCANView->EnableEditing(false);
            gridCANView->EnableGridLines(true);
            gridCANView->EnableDragGridSize(false);
            gridCANView->SetMargins(0, 0);
            // параметры столбцов
            gridCANView->EnableDragColMove(false);
            gridCANView->EnableDragColSize(true);
            gridCANView->SetColLabelSize(FromDIP(20));
            gridCANView->SetColLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
            // параметры строк
            gridCANView->EnableDragRowSize(false);
            gridCANView->SetRowLabelSize(FromDIP(40));
            gridCANView->SetRowLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
            gridCANView->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
            // заполнение таблицы
            gridCANView->SetColLabelValue(0, wxT("CAN ID"));
            gridCANView->SetColLabelValue(1, wxT("Интервал"));
            gridCANView->SetColLabelValue(2, wxT("Длина"));
            gridCANView->SetColLabelValue(3, wxT("Байт 0"));
            gridCANView->SetColLabelValue(4, wxT("Байт 1"));
            gridCANView->SetColLabelValue(5, wxT("Байт 2"));
            gridCANView->SetColLabelValue(6, wxT("Байт 3"));
            gridCANView->SetColLabelValue(7, wxT("Байт 4"));
            gridCANView->SetColLabelValue(8, wxT("Байт 5"));
            gridCANView->SetColLabelValue(9, wxT("Байт 6"));
            gridCANView->SetColLabelValue(10, wxT("Байт 7"));
            // установка ширины столбцов
            gridCANView->SetColSize(0, FromDIP(100));
            for (size_t iCol = 1; iCol < 11; iCol++)
            {
                gridCANView->SetColSize(iCol, FromDIP(60));
            }
            // внешний вид
            gridCANView->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectNone);
            gridCANView->SetDefaultCellTextColour(themeColors.GridFont);
            gridCANView->SetDefaultCellBackgroundColour(themeColors.GridBackground);
            gridCANView->SetGridLineColour(themeColors.GridLines);
        }
        sizerLeftTop->Add(gridCANView, 1, wxEXPAND, 0);
        panelLeftTop->SetSizer(sizerLeftTop);

        // левый нижний сайзер
        auto sizerLeftBottom = new wxStaticBoxSizer(new wxStaticBox(panelLeftBottom, wxID_ANY, wxT("Тестовая отправка пакета и просмотр ответа")), wxVERTICAL);
        {
            // сайзер с текстовыми полями ввода данных CAN-пакета
            auto sizerLeftBottomText = new wxBoxSizer(wxHORIZONTAL);
            {
                auto labelCAN = new wxStaticText(panelLeftBottom, wxID_ANY, wxT("CAN:"), wxDefaultPosition, FromDIP(wxSize(37, 20)));
                labelCAN->Wrap(-1);
                sizerLeftBottomText->Add(labelCAN, 0, wxLEFT | wxTOP, 2);
                textCANID = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("7E0"), wxDefaultPosition, FromDIP(wxSize(51, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                sizerLeftBottomText->Add(textCANID, 0, wxEXPAND, 0);
                textCANLength = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("8"), wxDefaultPosition, FromDIP(wxSize(50, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                sizerLeftBottomText->Add(textCANLength, 0, wxEXPAND, 0);
                textCANByte1 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("05"), wxDefaultPosition, FromDIP(wxSize(50, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                sizerLeftBottomText->Add(textCANByte1, 0, wxEXPAND, 0);
                textCANByte2 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("2F"), wxDefaultPosition, FromDIP(wxSize(50, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                sizerLeftBottomText->Add(textCANByte2, 0, wxEXPAND, 0);
                textCANByte3 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("00"), wxDefaultPosition, FromDIP(wxSize(50, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                sizerLeftBottomText->Add(textCANByte3, 0, wxEXPAND, 0);
                textCANByte4 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("0A"), wxDefaultPosition, FromDIP(wxSize(50, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                sizerLeftBottomText->Add(textCANByte4, 0, wxEXPAND, 0);
                textCANByte5 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("06"), wxDefaultPosition, FromDIP(wxSize(50, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                sizerLeftBottomText->Add(textCANByte5, 0, wxEXPAND, 0);
                textCANByte6 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("FF"), wxDefaultPosition, FromDIP(wxSize(50, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                sizerLeftBottomText->Add(textCANByte6, 0, wxEXPAND, 0);
                textCANByte7 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("00"), wxDefaultPosition, FromDIP(wxSize(50, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                sizerLeftBottomText->Add(textCANByte7, 0, wxEXPAND, 0);
                textCANByte8 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("00"), wxDefaultPosition, FromDIP(wxSize(50, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                sizerLeftBottomText->Add(textCANByte8, 0, wxEXPAND, 0);
            }
            sizerLeftBottom->Add(sizerLeftBottomText, 0, wxEXPAND, 0);

            // сайзер с кнопками управления отправкой и журналом ответа
            auto sizerLeftBottomButtons = new wxBoxSizer(wxHORIZONTAL);
            {
                buttonSend = new wxButton(panelLeftBottom, wxID_ANY, wxT("Отправить CAN-пакет"));
                sizerLeftBottomButtons->Add(buttonSend, 0, wxALL, 4);
                auto labelCANFromID = new wxStaticText(panelLeftBottom, wxID_ANY, wxT("Отображать ответ от:"), wxDefaultPosition, wxDefaultSize);
                labelCANFromID->Wrap(-1);
                sizerLeftBottomButtons->Add(labelCANFromID, 0, wxALL, 9);
                textCANAnswerID = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("7E8"), wxDefaultPosition, FromDIP(wxSize(51, 20)), wxTE_CENTRE | wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
                sizerLeftBottomButtons->Add(textCANAnswerID, 0, wxALL, 7);
                // спейсер между кнопками
                sizerLeftBottomButtons->Add(0, 0, 1, wxEXPAND, 0);
                buttonClearCANLog = new wxButton(panelLeftBottom, wxID_ANY, wxT("Очистить ответ"));
                sizerLeftBottomButtons->Add(buttonClearCANLog, 0, wxALL, 4);
            }
            sizerLeftBottom->Add(sizerLeftBottomButtons, 0, wxEXPAND, 0);

            gridCANLog = new wxGrid(panelLeftBottom, wxID_ANY);
            // параметры сетки
            gridCANLog->CreateGrid(0, 10);
            gridCANLog->EnableEditing(false);
            gridCANLog->EnableGridLines(true);
            gridCANLog->EnableDragGridSize(false);
            gridCANLog->SetMargins(0, 0);
            // параметры столбцов
            gridCANLog->EnableDragColMove(false);
            gridCANLog->EnableDragColSize(false);
            gridCANLog->SetColLabelSize(FromDIP(20));
            gridCANLog->SetColLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
            // параметры строк
            gridCANLog->EnableDragRowSize(false);
            gridCANLog->SetRowLabelSize(FromDIP(40));
            gridCANLog->SetRowLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
            gridCANLog->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
            // заполнение таблицы
            gridCANLog->SetColLabelValue(0, wxT("CAN ID"));
            gridCANLog->SetColLabelValue(1, wxT("Длина"));
            gridCANLog->SetColLabelValue(2, wxT("Байт 0"));
            gridCANLog->SetColLabelValue(3, wxT("Байт 1"));
            gridCANLog->SetColLabelValue(4, wxT("Байт 2"));
            gridCANLog->SetColLabelValue(5, wxT("Байт 3"));
            gridCANLog->SetColLabelValue(6, wxT("Байт 4"));
            gridCANLog->SetColLabelValue(7, wxT("Байт 5"));
            gridCANLog->SetColLabelValue(8, wxT("Байт 6"));
            gridCANLog->SetColLabelValue(9, wxT("Байт 7"));
            // установка ширины столбцов
            for (size_t iCol = 0; iCol < 10; iCol++)
            {
                gridCANLog->SetColSize(iCol, FromDIP(60));
            }
            gridCANLog->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectNone);
            // внешний вид
            gridCANLog->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectNone);
            gridCANLog->SetDefaultCellTextColour(themeColors.GridFont);
            gridCANLog->SetDefaultCellBackgroundColour(themeColors.GridBackground);
            gridCANLog->SetGridLineColour(themeColors.GridLines);
            sizerLeftBottom->Add(gridCANLog, 1, wxEXPAND, 0);
        }
        panelLeftBottom->SetSizer(sizerLeftBottom);

        splitterLeft->SplitHorizontally(panelLeftTop, panelLeftBottom, -1);
        splitterLeft->SetMinimumPaneSize(FromDIP(30));
        // пропорция задаёт ширину левой области
        sizerMain->Add(splitterLeft, 1, wxALL | wxEXPAND, 4);

        // правый сайзер
        auto sizerRight = new wxBoxSizer(wxVERTICAL);
        {
            // последовательный порт, кнопка управления и статистика буфера
            auto sizerControls = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Управление")), wxHORIZONTAL);
            {
                comboBoxSerialPort = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(90, 22)), 0, nullptr, wxTE_CENTRE | wxBORDER_SIMPLE);
                auto ports = ThreadedSerialPort::Enumerate();
                wxString serialPortToolTip;
                if (ports.size() > 0)
                {
                    for (auto& port : ports)
                    {
                        comboBoxSerialPort->Append(port.Port);
                        if (port.HardwareID.IsEmpty())
                        {
                            serialPortToolTip += port.Port + wxT("\n  ") + port.Description + wxT("\n\n");
                        }
                        else
                        {
                            serialPortToolTip += port.Port + wxT("\n  ") + port.Description + wxT("\n  ") + port.HardwareID + wxT("\n\n");
                        }
                    }
                    serialPortToolTip.RemoveLast(2);
                    // по умолчанию выбрать первый порт из списка
                    if (ports.size() > 0)
                    {
                        comboBoxSerialPort->Select(0);
                    }
                    comboBoxSerialPort->SetToolTip(serialPortToolTip);
                }
                else
                {
                    comboBoxSerialPort->SetToolTip(wxT("Последовательный порт"));
                }
                sizerControls->Add(comboBoxSerialPort, 2, wxALL, 2);

                comboBoxSerialSpeed = new wxComboBox(this, wxID_ANY, wxT("500000"), wxDefaultPosition, FromDIP(wxSize(90, 22)), 0, nullptr, wxTE_CENTRE | wxBORDER_SIMPLE);
                comboBoxSerialSpeed->Append(wxT("57600"));
                comboBoxSerialSpeed->Append(wxT("115200"));
                comboBoxSerialSpeed->Append(wxT("250000"));
                comboBoxSerialSpeed->Append(wxT("500000"));
                comboBoxSerialSpeed->Append(wxT("1000000"));
                comboBoxSerialSpeed->Append(wxT("2000000"));
                comboBoxSerialSpeed->SetToolTip(wxT("Скорость соединения"));
                sizerControls->Add(comboBoxSerialSpeed, 2, wxALL, 2);

                buttonConnectDisconnect = new wxButton(this, wxID_ANY, wxT("Подключить"), wxDefaultPosition, FromDIP(wxSize(80, 25)));
                buttonConnectDisconnect->SetFocus();
                sizerControls->Add(buttonConnectDisconnect, 2, wxALL, 0);

                textFPS = new wxTextCtrl(this, wxID_ANY, wxT("0"), wxDefaultPosition, FromDIP(wxSize(50, 22)), wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
                textFPS->SetToolTip(wxT("пакетов/с"));
                sizerControls->Add(textFPS, 1, wxALL, 2);

                textBPS = new wxTextCtrl(this, wxID_ANY, wxT("0"), wxDefaultPosition, FromDIP(wxSize(50, 22)), wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
                textBPS->SetToolTip(wxT("байтов/с (исключая служебную информацию)"));
                sizerControls->Add(textBPS, 1, wxALL, 2);
            }
            sizerRight->Add(sizerControls, 0, wxALL | wxEXPAND, 2);

            // журнал, кнопки управления логом и его параметры
            paneLog = new wxCollapsiblePane(this, wxID_ANY, "Запись в журнал", wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE | wxCP_DEFAULT_STYLE);
            paneLog->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent&)
                {
                    Layout();
                    Refresh();
                });
            {
                auto pane = paneLog->GetPane();
                auto sizerLog = new wxBoxSizer(wxVERTICAL);
                {
                    checkLogEnable = new wxCheckBox(pane, wxID_ANY, wxT("Вести запись в журнал"));
                    checkLogEnable->SetValue(logEnable);
                    sizerLog->Add(checkLogEnable, 0, wxALL, 5);

                    auto sizerLogButtonsList = new wxBoxSizer(wxHORIZONTAL);
                    {
                        // кнопки журнала
                        auto sizerLogButtons = new wxBoxSizer(wxVERTICAL);
                        {
                            buttonAdd = new wxButton(pane, wxID_ANY, wxT("Добавить ID в фильтр >>"));
                            sizerLogButtons->Add(buttonAdd, 1, wxALL | wxEXPAND, 2);

                            buttonRemove = new wxButton(pane, wxID_ANY, wxT("Убрать ID из фильтра <<"));
                            sizerLogButtons->Add(buttonRemove, 1, wxALL | wxEXPAND, 2);

                            buttonRemoveAll = new wxButton(pane, wxID_ANY, wxT("Очистить фильтр"));
                            sizerLogButtons->Add(buttonRemoveAll, 1, wxALL | wxEXPAND, 2);

                            // ширина кнопок фильтров
                            sizerLogButtonsList->Add(sizerLogButtons, 6, wxALL, 2);
                        }
                        // список кадров журнала
                        listLog = new wxListBox(pane, wxID_ANY);
                        // ширина списка фильтров
                        sizerLogButtonsList->Add(listLog, 4, wxALL | wxEXPAND, 4);
                    }
                    sizerLog->Add(sizerLogButtonsList, 0, wxEXPAND, 2);

                    // настройки типа файла журнала
                    auto sizerLogType = new wxBoxSizer(wxHORIZONTAL);
                    {
                        auto labelExt = new wxStaticText(pane, wxID_ANY, wxT("Тип файла:"));
                        labelExt->Wrap(-1);
                        sizerLogType->Add(labelExt, 0, wxALL, 8);

                        wxArrayString comboExtChoices;
                        comboExtChoices.Add(wxT(".csv"));
                        comboExtChoices.Add(wxT(".log"));
                        choiceExt = new wxChoice(pane, wxID_ANY, wxDefaultPosition, wxDefaultSize, comboExtChoices);
                        choiceExt->SetSelection(0);
                        logExt = wxT(".csv");
                        sizerLogType->Add(choiceExt, 0, wxALL, 2);

                        auto labelSep = new wxStaticText(pane, wxID_ANY, wxT("Разделитель:"));
                        labelSep->Wrap(-1);
                        sizerLogType->Add(labelSep, 0, wxALL, 8);

                        wxArrayString comboSepChoices;
                        comboSepChoices.Add(wxT(";"));
                        comboSepChoices.Add(wxT("Tab"));
                        comboSepChoices.Add(wxT("-"));
                        comboSepChoices.Add(wxT("_"));
                        comboSepChoices.Add(wxT("Space"));

                        choiceSep = new wxChoice(pane, wxID_ANY, wxDefaultPosition, wxDefaultSize, comboSepChoices);
                        choiceSep->SetSelection(0);
                        logSeparator = wxT(";");
                        sizerLogType->Add(choiceSep, 0, wxALL, 2);
                    }
                    sizerLog->Add(sizerLogType, 0, wxALIGN_CENTER, 2);

                    // параметры сохранения данных в лог
                    auto sizerLogParameters = new wxBoxSizer(wxVERTICAL);
                    {
                        checkSingle = new wxCheckBox(pane, wxID_ANY, wxT("Сохранение данных в один файл"));
                        checkSingle->SetValue(logSingle);
                        sizerLogParameters->Add(checkSingle, 0, wxALL, 5);
                        checkDec = new wxCheckBox(pane, wxID_ANY, wxT("Десятичный вывод данных"));
                        checkDec->SetValue(logDecimal);
                        sizerLogParameters->Add(checkDec, 0, wxALL, 5);
                        checkASCII = new wxCheckBox(pane, wxID_ANY, wxT("Добавлять ASCII данные"));
                        checkASCII->SetValue(logASCII);
                        sizerLogParameters->Add(checkASCII, 0, wxALL, 5);
                    }
                    sizerLog->Add(sizerLogParameters, 0, wxALIGN_LEFT, 2);

                    pane->SetSizer(sizerLog);
                }
                sizerLog->SetSizeHints(pane);
            }
            sizerRight->Add(paneLog, 0, wxALL | wxEXPAND, 4);

            // элементы для наглядного представления чисел
            auto sizerDecoders = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Декодированные значения")), wxVERTICAL);
            {
                // выбор типа данных для отображения и порядка следования байтов
                auto sizerDataType = new wxBoxSizer(wxHORIZONTAL);
                {
                    auto labelDataType = new wxStaticText(this, wxID_ANY, wxT("Тип данных:"), wxDefaultPosition, wxDefaultSize);
                    labelDataType->Wrap(-1);
                    sizerDataType->Add(labelDataType, 1, wxALL, 3);
                    wxArrayString comboDataTypeChoices;
                    comboDataTypeChoices.Add(TEXT_UINT8);
                    comboDataTypeChoices.Add(TEXT_UINT16);
                    comboDataTypeChoices.Add(TEXT_UINT32);
                    comboDataTypeChoices.Add(TEXT_INT8);
                    comboDataTypeChoices.Add(TEXT_INT16);
                    comboDataTypeChoices.Add(TEXT_INT32);
                    comboDataTypeChoices.Add(TEXT_FLOAT);
                    choiceDataType = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, comboDataTypeChoices);
                    choiceDataType->Select(0);
                    sizerDataType->Add(choiceDataType, 1, wxALL, 0);
                    sizerDataType->Add(0, 0, 1, wxALL, 0);
                    checkEndian = new wxCheckBox(this, wxID_ANY, wxT("Big-endian"));
                    checkEndian->SetValue(bigEndian);
                    checkEndian->Disable();
                    sizerDataType->Add(checkEndian, 1, wxALL, 5);
                }
                sizerDecoders->Add(sizerDataType, 0, wxALL | wxEXPAND, 2);

                // отображение двоичного байта
                auto sizerDecoderBinary = new wxBoxSizer(wxHORIZONTAL);
                {
                    auto labelBinary = new wxStaticText(this, wxID_ANY, wxT("Двоичный вид:"), wxDefaultPosition, wxDefaultSize);
                    labelBinary->Wrap(-1);
                    sizerDecoderBinary->Add(labelBinary, 1, wxALL, 3);
                    textBin = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
                    sizerDecoderBinary->Add(textBin, 3, wxALL, 0);
                }
                sizerDecoders->Add(sizerDecoderBinary, 0, wxALL | wxEXPAND, 2);

                // отображение десятичного байта
                auto sizerDecoderDecimal = new wxBoxSizer(wxHORIZONTAL);
                {
                    auto labelDecimal = new wxStaticText(this, wxID_ANY, wxT("Десятичный вид:"), wxDefaultPosition, wxDefaultSize);
                    labelDecimal->Wrap(-1);
                    sizerDecoderDecimal->Add(labelDecimal, 1, wxALL, 3);
                    textDec = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
                    sizerDecoderDecimal->Add(textDec, 3, wxALL, 0);
                }
                sizerDecoders->Add(sizerDecoderDecimal, 0, wxALL | wxEXPAND, 2);

                // множитель десятичного слова
                auto sizerDecimalMul = new wxBoxSizer(wxHORIZONTAL);
                {
                    auto labelDecimalMul = new wxStaticText(this, wxID_ANY, wxT("Множитель:"), wxDefaultPosition, wxDefaultSize);
                    labelDecimalMul->Wrap(-1);
                    sizerDecimalMul->Add(labelDecimalMul, 1, wxALL, 3);
                    textDecWordMul = new wxTextCtrl(this, wxID_ANY, wxString::Format(FORMAT_FLOAT1_3, mul), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE | wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
                    sizerDecimalMul->Add(textDecWordMul, 3, wxALL, 0);
                }
                sizerDecoders->Add(sizerDecimalMul, 0, wxALL | wxEXPAND, 2);

                // результат умножения десятичного слова
                auto sizerDecimalResult = new wxBoxSizer(wxHORIZONTAL);
                {
                    auto labelDecimalResult = new wxStaticText(this, wxID_ANY, wxT("Результат:"), wxDefaultPosition, wxDefaultSize);
                    labelDecimalResult->Wrap(-1);
                    sizerDecimalResult->Add(labelDecimalResult, 1, wxALL, 3);
                    textDecimalResult = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
                    sizerDecimalResult->Add(textDecimalResult, 3, wxALL, 0);
                }
                sizerDecoders->Add(sizerDecimalResult, 0, wxALL | wxEXPAND, 2);
            }
            sizerRight->Add(sizerDecoders, 0, wxALL | wxEXPAND, 2);

            // панель для графика
            drawPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
            drawPanel->SetBackgroundStyle(wxBG_STYLE_PAINT);
            drawPanel->SetBackgroundColour(*wxWHITE);
            
            sizerRight->Add(drawPanel, 1, wxALL | wxEXPAND, 2);
        }
        // ширина правой области
        sizerMain->Add(sizerRight, 0, wxALL | wxEXPAND, 2);
    }

    this->SetSizer(sizerMain);
    this->SetAutoLayout(true);
    this->Layout();
    this->Center(wxCENTER_ON_SCREEN);
    //this->SetDoubleBuffered(true);
    this->SetBackgroundColour(themeColors.WindowBackground);

    // привязка событий
    this->Bind(wxEVT_CLOSE_WINDOW, &FormMain::OnClose, this, ID_MAIN_FORM);
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
    drawPanel->Bind(wxEVT_SIZE, &FormMain::DrawPanel_OnSize, this);
    drawPanel->Bind(wxEVT_ERASE_BACKGROUND, &FormMain::DrawPanel_OnEraseBackground, this);
    drawFrameSize = drawPanel->GetClientSize().GetWidth();
    drawData = new CircularFrameBuffer(drawFrameSize);

    // событие от фонового потока COM-порта
    this->Bind(wxEVT_SERIAL_PORT_THREAD_UPDATE, &FormMain::Thread_OnUpdate, this);
    this->Bind(wxEVT_SERIAL_PORT_THREAD_EXIT, &FormMain::Thread_OnExit, this);
    this->Bind(wxEVT_SERIAL_PORT_THREAD_MESSAGE, &FormMain::Thread_OnMessage, this);

    // так и не понял, как прикрепить обработчик события для сокета, поэтому он отдельно в таблице событий выше
    //this->Bind(wxEVT_SOCKET, &FormMain::UDPSocket_OnEvent, this, ID_UDP_SOCKET);

    // событие таймера
    this->Bind(wxEVT_TIMER, &FormMain::MainTimer_OnTimer, this, ID_MAIN_TIMER);

    // создание хранилища пакетов
    frames = new FramesContainer(themeColors);

    // настройка и запуск таймера
    timerMain = new wxTimer(this, ID_MAIN_TIMER);
    timerMain->Start(TIMER_INTERVAL);

    // определение разделителя дробной части чисел
    /*const wxLanguageInfo* langInfo = wxLocale::GetLanguageInfo(wxLocale::GetSystemLanguage());
    wxChar sepBuf[256] { 0 };
    GetLocaleInfo(langInfo->GetLCID(), LOCALE_SDECIMAL, sepBuf, WXSIZEOF(sepBuf));
    decimalSeparator = sepBuf;*/

    // для русского языка определяется запятая, как и положено
    // эта запятая и ставится в дробную часть времени в файл, разделяя секунды и миллисекунды
    // вот только Excel не понимает этого и неверно отображает миллисекунды
    // поэтому разделителем ставлю точку
    decimalSeparator = wxT(".");

    // подготовка сетевых операций
    espIpAddress.Service(UDP_PORT);

    wxIPV4address ipAddress;
    ipAddress.AnyAddress();
    ipAddress.Service(UDP_PORT);
    udpSocket = new wxDatagramSocket(ipAddress);

    if (udpSocket->IsOk())
    {
        udpSocket->SetEventHandler(*this, ID_UDP_SOCKET);
        udpSocket->SetNotify(wxSOCKET_INPUT_FLAG);
        udpSocket->Notify(true);
    }
    else if (udpSocket->Error())
    {
        wxMessageBox(ERROR_UDP_OPEN + udpSocket->LastError());
    }
}

// Нажатие кнопки закрытия окна
void FormMain::OnClose(wxCloseEvent& event)
{
    timerMain->Stop();

    if (udpSocket)
    {
        udpSocket->Destroy();
        udpSocket = nullptr;
    }

    if (serialPort)
    {
        serialPort->Delete();
        serialPort = nullptr;
    }

    if (drawData)
    {
        delete drawData;
        drawData = nullptr;
    }

    if (frames)
    {
        delete frames;
        frames = nullptr;
    }

    // записать все файлы
    FlushLogs();

    Destroy();
}

// Обработчик кнопки Подключить/отключить
void FormMain::ButtonConnectDisconnect_OnClick(wxCommandEvent& event)
{
    rowToView = -1;
    colToView = -1;
    drawData->Clear();

    try
    {
        // если порт не открыт - открыть, иначе - закрыть
        if (serialPort == nullptr)
        {
            unsigned long serialSpeed = 0;
            if (comboBoxSerialSpeed->GetValue().ToULong(&serialSpeed))
            {
                frames->Clear();

                // удалить все строки таблицы
                if (gridCANView->GetNumberRows() > 0)
                {
                    gridCANView->DeleteRows(0, gridCANView->GetNumberRows());
                }

                serialPort = new ThreadedSerialPort(comboBoxSerialPort->GetValue(), (uint32_t)serialSpeed, (wxFrame*)this);
                // поток стартует с задержкой, все остальные проверки его состояния в событии таймера MainTimer_OnTimer
                buttonConnectDisconnect->SetLabelText(DISCONNECT);
            }
        }
        else
        {
            // просто запустить процесс остановки потока, все остальные проверки его состояния в событии таймера MainTimer_OnTimer
            serialPort->Delete();
            // записать все log-файлы
            FlushLogs();
            logFiles.clear();
        }
    }
    catch (...)
    {
        buttonConnectDisconnect->SetLabelText(CONNECT);
        wxMessageBox(ERROR_SERIAL);
    }
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
    buttonConnectDisconnect->SetLabelText(CONNECT);
}

// Сообщение от потока при ошибке
void FormMain::Thread_OnMessage(wxThreadEvent& event)
{
    wxMessageBox(event.GetPayload<wxString>(), ERROR_CAPTION, wxICON_ERROR);
}

// Обработка поступившего CAN-пакета
void FormMain::ProcessCANFrame(CANFrameIn& frame)
{
    bool found = false;

    // если это пакет с адресом 000 - это статистика и её надо вывести отдельно
    if (frame.id == 0 && frame.length >= 4)
    {
        uint16_t fps = ((uint16_t)frame.data[0] << 8) + (uint16_t)frame.data[1];
        textFPS->ChangeValue(wxString::Format(FORMAT_INT, fps));    // кадров в секунду
        uint16_t bps = ((uint16_t)frame.data[2] << 8) + (uint16_t)frame.data[3];
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

            gridCANLog->SetCellValue(lastRow, 0, wxString::Format(FORMAT_HEX3, frame.id));
            gridCANLog->SetCellValue(lastRow, 1, wxString::Format(FORMAT_INT, frame.length));

            // заполнение столбцов параметров
            for (size_t iData = 0; iData < 8; iData++)
            {
                if (iData < frame.length)
                {
                    // вывод данных
                    gridCANLog->SetCellValue(lastRow, iData + 2, wxString::Format(FORMAT_HEX2, frame.data[iData]));
                }
                else
                {
                    // вывод пустых ячеек
                    gridCANView->SetCellValue(lastRow, iData + 2, wxT(" "));
                }
            }
            // прокрутить список вниз
            gridCANLog->MakeCellVisible(gridCANLog->GetNumberRows() - 1, 0);
        }
    }
}

// Выбран другой тип данных для отображения
void FormMain::ChoiceDataType_OnChoice(wxCommandEvent& event)
{
    dataType = (DataTypes)event.GetSelection();
    checkEndian->Enable(dataType != DataTypes::UInt8 && dataType != DataTypes::Int8);
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
    if (value == wxT("Tab"))
        logSeparator = wxT("\t");
    else if (value == wxT("Space"))
        logSeparator = wxT(" ");
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
                    auto logPath = wxGetCwd() + wxFILE_SEP_PATH + wxT("CAN") + logExt;
                    logFile = new wxFFile();
                    if (logFile->Open(logPath, wxT("a")))
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
                    auto logPath = wxGetCwd() + wxT("/CAN ID ") + wxString::Format(FORMAT_HEX3, frame.id) + logExt;

                    LogFile newLogFile = { 0 };
                    newLogFile.file = new wxFFile();
                    if (newLogFile.file->Open(logPath, wxT("a")))
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
            newLine += '|' + logSeparator;

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

        textBin->ChangeValue(wxT(""));
        textDec->ChangeValue(wxT(""));
        textDecimalResult->ChangeValue(wxT(""));
    }
    else
    {
        // показать числа в разных форматах
        ShowNumbers();
    }
}

// Преобразовывает байт в двоичное представление
wxString FormMain::ToBinary(uint8_t value)
{
    wxString binaryString;

    for (size_t counter = 0; counter < 4; counter++)
    {
        binaryString += (value & 0x80) ? wxT('1') : wxT('0');
        value <<= 1;
    }
    binaryString += wxT('_');
    for (size_t counter = 0; counter < 4; counter++)
    {
        binaryString += (value & 0x80) ? wxT('1') : wxT('0');
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
        binaryString += (value & 0x8000) ? wxT('1') : wxT('0');
        value <<= 1;
    }
    binaryString += wxT('_');
    for (size_t counter = 0; counter < 8; counter++)
    {
        binaryString += (value & 0x8000) ? wxT('1') : wxT('0');
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
        binaryString += (value & 0x80000000) ? wxT('1') : wxT('0');
        value <<= 1;
    }
    binaryString += wxT('_');
    for (size_t counter = 0; counter < 8; counter++)
    {
        binaryString += (value & 0x80000000) ? wxT('1') : wxT('0');
        value <<= 1;
    }
    binaryString += wxT('_');
    for (size_t counter = 0; counter < 8; counter++)
    {
        binaryString += (value & 0x80000000) ? wxT('1') : wxT('0');
        value <<= 1;
    }
    binaryString += wxT('_');
    for (size_t counter = 0; counter < 8; counter++)
    {
        binaryString += (value & 0x80000000) ? wxT('1') : wxT('0');
        value <<= 1;
    }
    return binaryString;
}

// Срабатывание таймера
void FormMain::MainTimer_OnTimer(wxTimerEvent& event)
{
    // проверка состояния последовательного порта
    if (serialPort && serialPort->hSerial == INVALID_HANDLE_VALUE)
    {
        delete serialPort;
        serialPort = nullptr;
        buttonConnectDisconnect->SetLabelText(CONNECT);
    }

    // обновить данные в таблице
    RefreshGridCANView();

    ShowNumbers();

    // это вызовет событие OnPaint для панели
    drawPanel->Refresh();
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
            gridCANView->SetCellValue(iFrame, 0, wxString::Format(FORMAT_HEX3, vFrame.frame.id));
            gridCANView->SetCellValue(iFrame, 1, wxString::Format(FORMAT_INT, vFrame.frame.interval));
            gridCANView->SetCellValue(iFrame, 2, wxString::Format(FORMAT_INT, vFrame.frame.length));
            gridCANView->SetCellBackgroundColour(iFrame, 0, themeColors.GridBackground);
            gridCANView->SetCellBackgroundColour(iFrame, 1, themeColors.GridBackground);
            gridCANView->SetCellBackgroundColour(iFrame, 2, themeColors.GridBackground);
            gridCANView->SetCellTextColour(iFrame, 0, themeColors.GridFont);
            gridCANView->SetCellTextColour(iFrame, 1, themeColors.GridFont);
            gridCANView->SetCellTextColour(iFrame, 2, themeColors.GridFont);

            // заполнение столбцов данных
            for (size_t iData = 0; iData < 8; iData++)
            {
                if (iData < vFrame.frame.length)
                {
                    // вывод данных с их фоновым цветом
                    gridCANView->SetCellValue(iFrame, iData + 3, wxString::Format(FORMAT_HEX2, vFrame.frame.data[iData]));
                    //auto color = vFrame.color[iData];
                    auto tunedColor = vFrame.color[iData].ChangeLightness(vFrame.lightness[iData]);
                    gridCANView->SetCellBackgroundColour(iFrame, iData + 3, tunedColor);
                    gridCANView->SetCellTextColour(iFrame, iData + 3, themeColors.GridFont);
                }
                else
                {
                    // вывод пустых ячеек
                    gridCANView->SetCellValue(iFrame, iData + 3, wxT(" "));
                    gridCANView->SetCellBackgroundColour(iFrame, iData + 3, themeColors.GridBackground);
                    gridCANView->SetCellTextColour(iFrame, iData + 3, themeColors.GridFont);
                }
            }
        }
        // раскраска выделенных ячеек
        if (colToView >= 0)
        {
            gridCANView->SetCellBackgroundColour(rowToView, colToView + 3, themeColors.GridSelectedBackground);
            switch (dataType)
            {
                case DataTypes::UInt16:
                case DataTypes::Int16:
                    if (colToView < 7)
                    {
                        gridCANView->SetCellBackgroundColour(rowToView, colToView + 4, themeColors.GridSelectedBackground);
                    }
                    break;

                case DataTypes::UInt32:
                case DataTypes::Int32:
                case DataTypes::Float:
                    if (colToView < 5)
                    {
                        gridCANView->SetCellBackgroundColour(rowToView, colToView + 4, themeColors.GridSelectedBackground);
                        gridCANView->SetCellBackgroundColour(rowToView, colToView + 5, themeColors.GridSelectedBackground);
                        gridCANView->SetCellBackgroundColour(rowToView, colToView + 6, themeColors.GridSelectedBackground);
                    }
                    else if (colToView < 6)
                    {
                        gridCANView->SetCellBackgroundColour(rowToView, colToView + 4, themeColors.GridSelectedBackground);
                        gridCANView->SetCellBackgroundColour(rowToView, colToView + 5, themeColors.GridSelectedBackground);
                    }
                    else if (colToView < 7)
                    {
                        gridCANView->SetCellBackgroundColour(rowToView, colToView + 4, themeColors.GridSelectedBackground);
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
        dc.SetPen(graphText);
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

        dc.SetTextForeground(graphText);
        auto fontMetrics = dc.GetFontMetrics();
        dc.DrawText(wxString::Format(FORMAT_FLOAT1_0, maxValue), fontMetrics.internalLeading, 0);
        dc.DrawText(wxString::Format(FORMAT_FLOAT1_0, minValue), fontMetrics.internalLeading, drawRectangle.height - fontMetrics.height - fontMetrics.descent);
    }
}

// Изменение размеров панели графика
void FormMain::DrawPanel_OnSize(wxSizeEvent& event)
{
    //drawRectangle = drawPanel->GetClientRect();
    //this->Refresh(true, &drawRectangle);
    //event.Skip();
}

// Стирание фона панели графика
void FormMain::DrawPanel_OnEraseBackground(wxEraseEvent& event)
{
    // кода нет, используется как заглушка
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
    long tempValue;

    // собрать CAN-пакет для отправки
    CANFrameOut frame = { 0 };

    // ID пакета
    textCANID->GetValue().ToLong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0x7FFF)
        frame.id = (uint32_t)tempValue;
    else
        return;
    // длина данных пакета
    textCANLength->GetValue().ToLong(&tempValue, 10);
    if (tempValue >= 0 && tempValue <= 8)
        frame.length = (uint8_t)tempValue;
    else
        return;
    // байт 1
    textCANByte1->GetValue().ToLong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[0] = (uint8_t)tempValue;
    else
        return;
    // байт 2
    textCANByte2->GetValue().ToLong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[1] = (uint8_t)tempValue;
    else
        return;
    // байт 3
    textCANByte3->GetValue().ToLong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[2] = (uint8_t)tempValue;
    else
        return;
    // байт 4
    textCANByte4->GetValue().ToLong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[3] = (uint8_t)tempValue;
    else
        return;
    // байт 5
    textCANByte5->GetValue().ToLong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[4] = (uint8_t)tempValue;
    else
        return;
    // байт 6
    textCANByte6->GetValue().ToLong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[5] = (uint8_t)tempValue;
    else
        return;
    // байт 7
    textCANByte7->GetValue().ToLong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[6] = (uint8_t)tempValue;
    else
        return;
    // байт 8
    textCANByte8->GetValue().ToLong(&tempValue, 16);
    if (tempValue >= 0 && tempValue <= 0xFF)
        frame.data[7] = (uint8_t)tempValue;
    else
        return;
    // запомнить ID пакета, от которого ожидается ответ
    textCANAnswerID->GetValue().ToLong(&tempValue, 16);
    if (tempValue > 0 && tempValue <= 0x7FFF)
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
    textCANAnswerID->GetValue().ToCULong((unsigned long*)&answerID, 16);
    textCANAnswerID->ChangeValue(wxString::Format(FORMAT_HEX3, answerID));
}

// Событие UDP-сокета
void FormMain::UDPSocket_OnEvent(wxSocketEvent& event)
{
    uint8_t  receivedData[UDP_BUFFER_SIZE] = { 0 };
    uint8_t* receivedDataPointer = receivedData;

    if (event.GetSocketEvent() == wxSOCKET_INPUT)
    {
        size_t receivedDataLen = udpSocket->RecvFrom(espIpAddress, receivedData, UDP_BUFFER_SIZE).LastCount();
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

// Отправить CAN-пакет через UDP-сокет
void FormMain::UDPSocket_SendFrame(CANFrameOut& frame)
{
    SendCANFrame sendCANFrame;
    sendCANFrame.Frame = frame;
    udpSocket->SendTo(espIpAddress, &sendCANFrame, sizeof(SendCANFrame));
}
