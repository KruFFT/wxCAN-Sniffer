#include "FormMain.h"

// Конструктор окна - создание и настройка контролов
FormMain::FormMain() : wxFrame(nullptr, IDs::MAIN_FORM, CAPTION, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE)
{
    gridSystemSelectedBackgroundColor = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);

    // сохранение цветов и настройка кистей
    graphFramePen = wxPen(Parameters::colors.GraphFrame, 1);
    graphBackgroundBrush = wxBrush(Parameters::colors.GraphBackground);
    graphPen = wxPen(Parameters::colors.GraphDraw, 3);

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

        // левый верхний сайзер -------------------------------------------------------------------
        auto labelLeftTop = new wxStaticBox(panelLeftTop, wxID_ANY, TEXT_CAN_PACKETS_VIEW);
        if (Parameters::appearance.ControlsCustomColors)
            labelLeftTop->SetForegroundColour(Parameters::colors.ControlText);
        auto sizerLeftTop = new wxStaticBoxSizer(labelLeftTop, wxVERTICAL);
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
            gridCANView->SetColLabelValue(0, TEXT_CAN_ID);
            gridCANView->SetColLabelValue(1, TEXT_CAN_INTERVAL);
            gridCANView->SetColLabelValue(2, TEXT_CAN_LENGTH);
            gridCANView->SetColLabelValue(3, TEXT_CAN_BYTE_0);
            gridCANView->SetColLabelValue(4, TEXT_CAN_BYTE_1);
            gridCANView->SetColLabelValue(5, TEXT_CAN_BYTE_2);
            gridCANView->SetColLabelValue(6, TEXT_CAN_BYTE_3);
            gridCANView->SetColLabelValue(7, TEXT_CAN_BYTE_4);
            gridCANView->SetColLabelValue(8, TEXT_CAN_BYTE_5);
            gridCANView->SetColLabelValue(9, TEXT_CAN_BYTE_6);
            gridCANView->SetColLabelValue(10, TEXT_CAN_BYTE_7);
            // установка ширины столбцов
            gridCANView->SetColSize(0, FromDIP(90));
            for (size_t iCol = 1; iCol < 11; iCol++)
            {
                gridCANView->SetColSize(iCol, FromDIP(60));
            }
            // внешний вид
            gridCANView->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectNone);
            if (Parameters::appearance.ControlsCustomColors)
            {
                gridCANView->SetDefaultCellTextColour(Parameters::colors.GridText);
                gridCANView->SetDefaultCellBackgroundColour(Parameters::colors.GridBackground);
                gridCANView->SetGridLineColour(Parameters::colors.GridLines);
                gridCANView->SetLabelTextColour(Parameters::colors.ControlText);
                gridCANView->SetLabelBackgroundColour(Parameters::colors.ControlBackground);
            }
        }
        sizerLeftTop->Add(gridCANView, 1, wxEXPAND, 0);
        panelLeftTop->SetSizer(sizerLeftTop);

        // левый нижний сайзер --------------------------------------------------------------------
        auto labelLeftBottom = new wxStaticBox(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST);
        if (Parameters::appearance.ControlsCustomColors)
            labelLeftBottom->SetForegroundColour(Parameters::colors.ControlText);
        auto sizerLeftBottom = new wxStaticBoxSizer(labelLeftBottom, wxVERTICAL);
        {
            // сайзер с текстовыми полями ввода данных CAN-пакета
            auto sizerLeftBottomText = new wxBoxSizer(wxHORIZONTAL);
            {
                auto labelCAN = new wxStaticText(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_PACKET, wxDefaultPosition, FromDIP(wxSize(70, 20)));
                if (Parameters::appearance.ControlsCustomColors)
                    labelCAN->SetForegroundColour(Parameters::colors.ControlText);
                labelCAN->Wrap(-1);
                sizerLeftBottomText->Add(labelCAN, 0, wxLEFT | wxTOP, 2);
                textCANID = new wxTextCtrl(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_ID, wxDefaultPosition, FromDIP(wxSize(70, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    textCANID->SetForegroundColour(Parameters::colors.ControlText);
                    textCANID->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                sizerLeftBottomText->Add(textCANID, 0, wxEXPAND, 0);
                textCANLength = new wxTextCtrl(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_LENGTH, wxDefaultPosition, FromDIP(wxSize(60, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    textCANLength->SetForegroundColour(Parameters::colors.ControlText);
                    textCANLength->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                sizerLeftBottomText->Add(textCANLength, 0, wxEXPAND, 0);
                textCANByte0 = new wxTextCtrl(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_BYTE_0, wxDefaultPosition, FromDIP(wxSize(60, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    textCANByte0->SetForegroundColour(Parameters::colors.ControlText);
                    textCANByte0->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                sizerLeftBottomText->Add(textCANByte0, 0, wxEXPAND, 0);
                textCANByte1 = new wxTextCtrl(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_BYTE_1, wxDefaultPosition, FromDIP(wxSize(60, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    textCANByte1->SetForegroundColour(Parameters::colors.ControlText);
                    textCANByte1->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                sizerLeftBottomText->Add(textCANByte1, 0, wxEXPAND, 0);
                textCANByte2 = new wxTextCtrl(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_BYTE_2, wxDefaultPosition, FromDIP(wxSize(60, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    textCANByte2->SetForegroundColour(Parameters::colors.ControlText);
                    textCANByte2->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                sizerLeftBottomText->Add(textCANByte2, 0, wxEXPAND, 0);
                textCANByte3 = new wxTextCtrl(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_BYTE_3, wxDefaultPosition, FromDIP(wxSize(60, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    textCANByte3->SetForegroundColour(Parameters::colors.ControlText);
                    textCANByte3->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                sizerLeftBottomText->Add(textCANByte3, 0, wxEXPAND, 0);
                textCANByte4 = new wxTextCtrl(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_BYTE_4, wxDefaultPosition, FromDIP(wxSize(60, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    textCANByte4->SetForegroundColour(Parameters::colors.ControlText);
                    textCANByte4->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                sizerLeftBottomText->Add(textCANByte4, 0, wxEXPAND, 0);
                textCANByte5 = new wxTextCtrl(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_BYTE_5, wxDefaultPosition, FromDIP(wxSize(60, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    textCANByte5->SetForegroundColour(Parameters::colors.ControlText);
                    textCANByte5->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                sizerLeftBottomText->Add(textCANByte5, 0, wxEXPAND, 0);
                textCANByte6 = new wxTextCtrl(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_BYTE_6, wxDefaultPosition, FromDIP(wxSize(60, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    textCANByte6->SetForegroundColour(Parameters::colors.ControlText);
                    textCANByte6->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                sizerLeftBottomText->Add(textCANByte6, 0, wxEXPAND, 0);
                textCANByte7 = new wxTextCtrl(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_BYTE_7, wxDefaultPosition, FromDIP(wxSize(60, 20)), wxTE_CENTRE | wxBORDER_SIMPLE);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    textCANByte7->SetForegroundColour(Parameters::colors.ControlText);
                    textCANByte7->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                sizerLeftBottomText->Add(textCANByte7, 0, wxEXPAND, 0);
            }
            sizerLeftBottom->Add(sizerLeftBottomText, 0, wxEXPAND, 0);

            // сайзер с кнопками управления отправкой и журналом ответа
            auto sizerLeftBottomButtons = new wxBoxSizer(wxHORIZONTAL);
            {
                auto labelCANFromID = new wxStaticText(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_ANSWER, wxDefaultPosition, wxDefaultSize);
                if (Parameters::appearance.ControlsCustomColors)
                    labelCANFromID->SetForegroundColour(Parameters::colors.ControlText);
                labelCANFromID->Wrap(-1);
                sizerLeftBottomButtons->Add(labelCANFromID, 0, wxLEFT | wxTOP, 2);
                textCANAnswerID = new wxTextCtrl(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_ANSWER_ID, wxDefaultPosition, FromDIP(wxSize(70, 20)), wxTE_CENTRE | wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    textCANAnswerID->SetForegroundColour(Parameters::colors.ControlText);
                    textCANAnswerID->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                sizerLeftBottomButtons->Add(textCANAnswerID, 0, wxALL, 7);
                buttonSend = new wxButton(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_SEND);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    buttonSend->SetForegroundColour(Parameters::colors.ControlText);
                    buttonSend->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                sizerLeftBottomButtons->Add(buttonSend, 0, wxALL, 4);
                // спейсер между кнопками
                //sizerLeftBottomButtons->Add(0, 0, 1, wxEXPAND, 0);
                buttonClearCANLog = new wxButton(panelLeftBottom, wxID_ANY, TEXT_CAN_TEST_CLEAR);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    buttonClearCANLog->SetForegroundColour(Parameters::colors.ControlText);
                    buttonClearCANLog->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
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
            gridCANLog->SetColLabelValue(0, TEXT_CAN_ID);
            gridCANLog->SetColLabelValue(1, TEXT_CAN_LENGTH);
            gridCANLog->SetColLabelValue(2, TEXT_CAN_BYTE_0);
            gridCANLog->SetColLabelValue(3, TEXT_CAN_BYTE_1);
            gridCANLog->SetColLabelValue(4, TEXT_CAN_BYTE_2);
            gridCANLog->SetColLabelValue(5, TEXT_CAN_BYTE_3);
            gridCANLog->SetColLabelValue(6, TEXT_CAN_BYTE_4);
            gridCANLog->SetColLabelValue(7, TEXT_CAN_BYTE_5);
            gridCANLog->SetColLabelValue(8, TEXT_CAN_BYTE_6);
            gridCANLog->SetColLabelValue(9, TEXT_CAN_BYTE_7);
            // установка ширины столбцов
            gridCANLog->SetColSize(0, FromDIP(100));
            for (size_t iCol = 1; iCol < 10; iCol++)
            {
                gridCANLog->SetColSize(iCol, FromDIP(60));
            }
            gridCANLog->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectNone);
            // внешний вид
            gridCANLog->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectNone);
            if (Parameters::appearance.ControlsCustomColors)
            {
                gridCANLog->SetDefaultCellTextColour(Parameters::colors.GridText);
                gridCANLog->SetDefaultCellBackgroundColour(Parameters::colors.GridBackground);
                gridCANLog->SetGridLineColour(Parameters::colors.GridLines);
                gridCANLog->SetLabelTextColour(Parameters::colors.ControlText);
                gridCANLog->SetLabelBackgroundColour(Parameters::colors.ControlBackground);
            }
            sizerLeftBottom->Add(gridCANLog, 1, wxEXPAND, 0);
        }
        panelLeftBottom->SetSizer(sizerLeftBottom);

        splitterLeft->SplitHorizontally(panelLeftTop, panelLeftBottom, -1);
        splitterLeft->SetMinimumPaneSize(FromDIP(30));

        // правый сайзер --------------------------------------------------------------------------
        auto sizerRight = new wxBoxSizer(wxVERTICAL);
        {
            // последовательный порт/сеть, кнопка управления и статистика буфера
            auto labelControls = new wxStaticBox(this, wxID_ANY, TEXT_MANAGEMENT);
            if (Parameters::appearance.ControlsCustomColors)
                labelControls->SetForegroundColour(Parameters::colors.ControlText);
            auto sizerControls = new wxStaticBoxSizer(labelControls, wxHORIZONTAL);
            {
                wxArrayString choiceModeList;
                choiceModeList.Add(TEXT_PORT);
                choiceModeList.Add(TEXT_NETWORK);

                choiceMode = new wxChoice(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(60, 27)), choiceModeList);
                choiceMode->SetToolTip(TEXT_CONNECTION_MODE);
                choiceMode->Select(0);

                sizerControls->Add(choiceMode, 0, wxALL, 2);

                comboBoxDataSource = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(100, 22)), 0, nullptr, wxBORDER_SIMPLE);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    comboBoxDataSource->SetForegroundColour(Parameters::colors.ControlText);
                    comboBoxDataSource->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                FillDataSource();
                sizerControls->Add(comboBoxDataSource, 0, wxALL, 2);

                comboSpeedChoices.Add(TEXT_CAN_SPEED_5);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_10);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_20);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_25);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_31);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_33);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_40);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_50);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_80);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_83);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_95);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_100);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_125);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_200);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_250);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_500);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_666);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_800);
                comboSpeedChoices.Add(TEXT_CAN_SPEED_1000);
                choiceCANSpeed = new wxChoice(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(60, 22)), comboSpeedChoices);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    choiceCANSpeed->SetForegroundColour(Parameters::colors.ControlText);
                    choiceCANSpeed->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                choiceCANSpeed->Select(15);
                choiceCANSpeed->SetToolTip(TEXT_CAN_SPEED);
                sizerControls->Add(choiceCANSpeed, 0, wxALL, 2);

                buttonConnectDisconnect = new wxButton(this, wxID_ANY, TEXT_CONNECT, wxDefaultPosition, FromDIP(wxSize(100, 25)));
                if (Parameters::appearance.ControlsCustomColors)
                {
                    buttonConnectDisconnect->SetForegroundColour(Parameters::colors.ControlText);
                    buttonConnectDisconnect->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                buttonConnectDisconnect->SetFocus();
                sizerControls->Add(buttonConnectDisconnect, 0, wxALL, 0);

                textFPS = new wxTextCtrl(this, wxID_ANY, TEXT_0, wxDefaultPosition, FromDIP(wxSize(50, 22)), wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    textFPS->SetForegroundColour(Parameters::colors.ControlText);
                    textFPS->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                textFPS->SetToolTip(TEXT_PPS);
                sizerControls->Add(textFPS, 0, wxALL, 2);

                textBPS = new wxTextCtrl(this, wxID_ANY, TEXT_0, wxDefaultPosition, FromDIP(wxSize(50, 22)), wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
                if (Parameters::appearance.ControlsCustomColors)
                {
                    textBPS->SetForegroundColour(Parameters::colors.ControlText);
                    textBPS->SetBackgroundColour(Parameters::colors.ControlBackground);
                }
                textBPS->SetToolTip(TEXT_BPS);
                sizerControls->Add(textBPS, 0, wxALL, 2);
            }

            // журнал, кнопки управления логом и его параметры
            paneLog = new wxCollapsiblePane(this, wxID_ANY, TEXT_LOG_WRITE, wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE | wxCP_DEFAULT_STYLE);
            // TODO: установка цвета для этого контрола не работает
            if (Parameters::appearance.ControlsCustomColors)
                paneLog->SetForegroundColour(Parameters::colors.ControlText);
            paneLog->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent&)
                {
                    Layout();
                    Refresh();
                });
            {
                auto pane = paneLog->GetPane();
                auto sizerLog = new wxBoxSizer(wxVERTICAL);
                {
                    checkLogEnable = new wxCheckBox(pane, wxID_ANY, TEXT_LOG_WRITE_ENABLE);
                    if (Parameters::appearance.ControlsCustomColors)
                        checkLogEnable->SetForegroundColour(Parameters::colors.ControlText);
                    checkLogEnable->SetValue(logEnable);
                    sizerLog->Add(checkLogEnable, 0, wxALL, 5);

                    auto sizerLogButtonsList = new wxBoxSizer(wxHORIZONTAL);
                    {
                        // кнопки журнала
                        auto sizerLogButtons = new wxBoxSizer(wxVERTICAL);
                        {
                            buttonAdd = new wxButton(pane, wxID_ANY, TEXT_LOG_ADD_ID);
                            if (Parameters::appearance.ControlsCustomColors)
                            {
                                buttonAdd->SetForegroundColour(Parameters::colors.ControlText);
                                buttonAdd->SetBackgroundColour(Parameters::colors.ControlBackground);
                            }
                            sizerLogButtons->Add(buttonAdd, 1, wxALL | wxEXPAND, 2);

                            buttonRemove = new wxButton(pane, wxID_ANY, TEXT_LOG_REMOVE_ID);
                            if (Parameters::appearance.ControlsCustomColors)
                            {
                                buttonRemove->SetForegroundColour(Parameters::colors.ControlText);
                                buttonRemove->SetBackgroundColour(Parameters::colors.ControlBackground);
                            }
                            sizerLogButtons->Add(buttonRemove, 1, wxALL | wxEXPAND, 2);

                            buttonRemoveAll = new wxButton(pane, wxID_ANY, TEXT_LOG_CLEAR_IDS);
                            if (Parameters::appearance.ControlsCustomColors)
                            {
                                buttonRemoveAll->SetForegroundColour(Parameters::colors.ControlText);
                                buttonRemoveAll->SetBackgroundColour(Parameters::colors.ControlBackground);
                            }
                            sizerLogButtons->Add(buttonRemoveAll, 1, wxALL | wxEXPAND, 2);

                            // ширина кнопок фильтров
                            sizerLogButtonsList->Add(sizerLogButtons, 6, wxALL, 2);
                        }
                        // список кадров журнала
                        listLog = new wxListBox(pane, wxID_ANY);
                        if (Parameters::appearance.ControlsCustomColors)
                        {
                            listLog->SetForegroundColour(Parameters::colors.ControlText);
                            listLog->SetBackgroundColour(Parameters::colors.ControlBackground);
                        }
                        // ширина списка фильтров
                        sizerLogButtonsList->Add(listLog, 4, wxALL | wxEXPAND, 4);
                    }
                    sizerLog->Add(sizerLogButtonsList, 0, wxEXPAND, 2);

                    // настройки типа файла журнала
                    auto sizerLogType = new wxBoxSizer(wxHORIZONTAL);
                    {
                        auto labelExt = new wxStaticText(pane, wxID_ANY, TEXT_LOG_FILE_TYPE);
                        if (Parameters::appearance.ControlsCustomColors)
                            labelExt->SetForegroundColour(Parameters::colors.ControlText);
                        labelExt->Wrap(-1);
                        sizerLogType->Add(labelExt, 0, wxALL, 8);

                        wxArrayString comboExtChoices;
                        comboExtChoices.Add(TEXT_LOG_FILE_EXT_CSV);
                        comboExtChoices.Add(TEXT_LOG_FILE_EXT_LOG);
                        choiceExt = new wxChoice(pane, wxID_ANY, wxDefaultPosition, wxDefaultSize, comboExtChoices);
                        if (Parameters::appearance.ControlsCustomColors)
                        {
                            choiceExt->SetForegroundColour(Parameters::colors.ControlText);
                            choiceExt->SetBackgroundColour(Parameters::colors.ControlBackground);
                        }
                        choiceExt->SetSelection(0);
                        logExt = TEXT_LOG_FILE_EXT_CSV;
                        sizerLogType->Add(choiceExt, 0, wxALL, 2);

                        auto labelSep = new wxStaticText(pane, wxID_ANY, TEXT_LOG_SEPARATOR);
                        if (Parameters::appearance.ControlsCustomColors)
                            labelSep->SetForegroundColour(Parameters::colors.ControlText);
                        labelSep->Wrap(-1);
                        sizerLogType->Add(labelSep, 0, wxALL, 8);

                        wxArrayString comboSepChoices;
                        comboSepChoices.Add(TEXT_LOG_SEPARATOR_SEMICOLON);
                        comboSepChoices.Add(TEXT_LOG_SEPARATOR_TAB);
                        comboSepChoices.Add(TEXT_LOG_SEPARATOR_MINUS);
                        comboSepChoices.Add(TEXT_LOG_SEPARATOR_UNDERSCORE);
                        comboSepChoices.Add(TEXT_LOG_SEPARATOR_SPACE);

                        choiceSep = new wxChoice(pane, wxID_ANY, wxDefaultPosition, wxDefaultSize, comboSepChoices);
                        if (Parameters::appearance.ControlsCustomColors)
                        {
                            choiceSep->SetForegroundColour(Parameters::colors.ControlText);
                            choiceSep->SetBackgroundColour(Parameters::colors.ControlBackground);
                        }
                        choiceSep->SetSelection(0);
                        logSeparator = TEXT_LOG_SEPARATOR_SEMICOLON;
                        sizerLogType->Add(choiceSep, 0, wxALL, 2);
                    }
                    sizerLog->Add(sizerLogType, 0, wxALIGN_CENTER, 2);

                    // параметры сохранения данных в лог
                    auto sizerLogParameters = new wxBoxSizer(wxVERTICAL);
                    {
                        checkSingle = new wxCheckBox(pane, wxID_ANY, TEXT_LOG_SAVE_TO_ONE_FILE);
                        if (Parameters::appearance.ControlsCustomColors)
                            checkSingle->SetForegroundColour(Parameters::colors.ControlText);
                        checkSingle->SetValue(logSingle);
                        sizerLogParameters->Add(checkSingle, 0, wxALL, 5);
                        checkDec = new wxCheckBox(pane, wxID_ANY, TEXT_LOG_DECIMAL);
                        if (Parameters::appearance.ControlsCustomColors)
                            checkDec->SetForegroundColour(Parameters::colors.ControlText);
                        checkDec->SetValue(logDecimal);
                        sizerLogParameters->Add(checkDec, 0, wxALL, 5);
                        checkASCII = new wxCheckBox(pane, wxID_ANY, TEXT_LOG_ADD_ASCII);
                        if (Parameters::appearance.ControlsCustomColors)
                            checkASCII->SetForegroundColour(Parameters::colors.ControlText);
                        checkASCII->SetValue(logASCII);
                        sizerLogParameters->Add(checkASCII, 0, wxALL, 5);
                    }
                    sizerLog->Add(sizerLogParameters, 0, wxALIGN_LEFT, 2);

                    pane->SetSizer(sizerLog);
                }
                sizerLog->SetSizeHints(pane);
            }

            // элементы для наглядного представления чисел
            auto labelDecoders = new wxStaticBox(this, wxID_ANY, TEXT_DECODING);
            if (Parameters::appearance.ControlsCustomColors)
                labelDecoders->SetForegroundColour(Parameters::colors.ControlText);
            auto sizerDecoders = new wxStaticBoxSizer(labelDecoders, wxVERTICAL);
            {
                // выбор типа данных для отображения и порядка следования байтов
                auto sizerDataType = new wxBoxSizer(wxHORIZONTAL);
                {
                    auto labelDataType = new wxStaticText(this, wxID_ANY, TEXT_DECODING_DATA_TYPE, wxDefaultPosition, wxDefaultSize);
                    labelDataType->SetForegroundColour(Parameters::colors.ControlText);
                    labelDataType->Wrap(-1);
                    sizerDataType->Add(labelDataType, 1, wxALL, 3);
                    wxArrayString comboDataTypeChoices;
                    comboDataTypeChoices.Add(TEXT_DECODING_UINT8);
                    comboDataTypeChoices.Add(TEXT_DECODING_UINT16);
                    comboDataTypeChoices.Add(TEXT_DECODING_UINT32);
                    comboDataTypeChoices.Add(TEXT_DECODING_INT8);
                    comboDataTypeChoices.Add(TEXT_DECODING_INT16);
                    comboDataTypeChoices.Add(TEXT_DECODING_INT32);
                    comboDataTypeChoices.Add(TEXT_DECODING_FLOAT);
                    choiceDataType = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, comboDataTypeChoices);
                    if (Parameters::appearance.ControlsCustomColors)
                    {
                        choiceDataType->SetForegroundColour(Parameters::colors.ControlText);
                        choiceDataType->SetBackgroundColour(Parameters::colors.ControlBackground);
                    }
                    choiceDataType->Select(0);
                    sizerDataType->Add(choiceDataType, 1, wxALL, 0);
                    sizerDataType->Add(0, 0, 1, wxALL, 0);
                    checkEndian = new wxCheckBox(this, wxID_ANY, TEXT_DECODING_BE);
                    if (Parameters::appearance.ControlsCustomColors)
                        checkEndian->SetForegroundColour(Parameters::colors.ControlText);
                    checkEndian->SetValue(bigEndian);
                    checkEndian->Disable();
                    sizerDataType->Add(checkEndian, 1, wxALL, 5);
                }
                sizerDecoders->Add(sizerDataType, 0, wxALL | wxEXPAND, 2);

                // отображение двоичного байта
                auto sizerDecoderBinary = new wxBoxSizer(wxHORIZONTAL);
                {
                    auto labelBinary = new wxStaticText(this, wxID_ANY, TEXT_DECODING_BINARY, wxDefaultPosition, wxDefaultSize);
                    if (Parameters::appearance.ControlsCustomColors)
                        labelBinary->SetForegroundColour(Parameters::colors.ControlText);
                    labelBinary->Wrap(-1);
                    sizerDecoderBinary->Add(labelBinary, 1, wxALL, 3);
                    textBin = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
                    if (Parameters::appearance.ControlsCustomColors)
                    {
                        textBin->SetForegroundColour(Parameters::colors.ControlText);
                        textBin->SetBackgroundColour(Parameters::colors.ControlBackground);
                    }
                    sizerDecoderBinary->Add(textBin, 3, wxALL, 0);
                }
                sizerDecoders->Add(sizerDecoderBinary, 0, wxALL | wxEXPAND, 2);

                // отображение десятичного байта
                auto sizerDecoderDecimal = new wxBoxSizer(wxHORIZONTAL);
                {
                    auto labelDecimal = new wxStaticText(this, wxID_ANY, TEXT_DECODING_DECIMAL, wxDefaultPosition, wxDefaultSize);
                    if (Parameters::appearance.ControlsCustomColors)
                        labelDecimal->SetForegroundColour(Parameters::colors.ControlText);
                    labelDecimal->Wrap(-1);
                    sizerDecoderDecimal->Add(labelDecimal, 1, wxALL, 3);
                    textDec = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
                    if (Parameters::appearance.ControlsCustomColors)
                    {
                        textDec->SetForegroundColour(Parameters::colors.ControlText);
                        textDec->SetBackgroundColour(Parameters::colors.ControlBackground);
                    }
                    sizerDecoderDecimal->Add(textDec, 3, wxALL, 0);
                }
                sizerDecoders->Add(sizerDecoderDecimal, 0, wxALL | wxEXPAND, 2);

                // множитель десятичного слова
                auto sizerDecimalMul = new wxBoxSizer(wxHORIZONTAL);
                {
                    auto labelDecimalMul = new wxStaticText(this, wxID_ANY, TEXT_DECODING_MULTIPLIER, wxDefaultPosition, wxDefaultSize);
                    if (Parameters::appearance.ControlsCustomColors)
                        labelDecimalMul->SetForegroundColour(Parameters::colors.ControlText);
                    labelDecimalMul->Wrap(-1);
                    sizerDecimalMul->Add(labelDecimalMul, 1, wxALL, 3);
                    textDecWordMul = new wxTextCtrl(this, wxID_ANY, wxString::Format(FORMAT_FLOAT1_3, mul), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE | wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
                    if (Parameters::appearance.ControlsCustomColors)
                    {
                        textDecWordMul->SetForegroundColour(Parameters::colors.ControlText);
                        textDecWordMul->SetBackgroundColour(Parameters::colors.ControlBackground);
                    }
                    sizerDecimalMul->Add(textDecWordMul, 3, wxALL, 0);
                }
                sizerDecoders->Add(sizerDecimalMul, 0, wxALL | wxEXPAND, 2);

                // результат умножения десятичного слова
                auto sizerDecimalResult = new wxBoxSizer(wxHORIZONTAL);
                {
                    auto labelDecimalResult = new wxStaticText(this, wxID_ANY, TEXT_DECODING_RESULT, wxDefaultPosition, wxDefaultSize);
                    if (Parameters::appearance.ControlsCustomColors)
                        labelDecimalResult->SetForegroundColour(Parameters::colors.ControlText);
                    labelDecimalResult->Wrap(-1);
                    sizerDecimalResult->Add(labelDecimalResult, 1, wxALL, 3);
                    textDecimalResult = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
                    if (Parameters::appearance.ControlsCustomColors)
                    {
                        textDecimalResult->SetForegroundColour(Parameters::colors.ControlText);
                        textDecimalResult->SetBackgroundColour(Parameters::colors.ControlBackground);
                    }
                    sizerDecimalResult->Add(textDecimalResult, 3, wxALL, 0);
                }
                sizerDecoders->Add(sizerDecimalResult, 0, wxALL | wxEXPAND, 2);
            }


            // панель для графика
            drawPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
            drawPanel->SetBackgroundStyle(wxBG_STYLE_PAINT);
            drawPanel->SetBackgroundColour(*wxWHITE);

            sizerRight->Add(sizerControls, 0, wxALL | wxEXPAND, 2);
            sizerRight->Add(paneLog, 0, wxALL | wxEXPAND, 4);
            sizerRight->Add(sizerDecoders, 0, wxALL | wxEXPAND, 2);
            sizerRight->Add(drawPanel, 1, wxALL | wxEXPAND, 2);
        }

        // пропорция задаёт ширину левой области
        sizerMain->Add(splitterLeft, 1, wxALL | wxEXPAND, 4);
        sizerMain->Add(sizerRight, 0, wxALL | wxEXPAND, 2);
    }

    this->SetSizer(sizerMain);
    this->SetAutoLayout(true);
    this->Layout();
    this->Center(wxCENTER_ON_SCREEN);
    if (Parameters::appearance.ControlsCustomColors)
    {
        this->SetBackgroundColour(Parameters::colors.WindowBackground);
    }
    else
    {
        this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
    }

    // почему-то установка двойной буфферизации корректно работает только после инициализации всех сайзеров
    gridCANView->SetDoubleBuffered(true);

    // параметры панели графика
    drawFrameSize = drawPanel->GetClientSize().GetWidth();
    drawData = new CircularFrameBuffer(drawFrameSize);

    // подключить обработчики событий
    AssignEventHandlers();

    // создание хранилища пакетов
    frames = new FramesContainer();

    // настройка и запуск таймера
    timerMain = new wxTimer(this, IDs::MAIN_TIMER);
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
    decimalSeparator = TEXT_DECIMAL_SEPARATOR;

    // подготовка сетевых операций
    mcIpAddress.Hostname(Parameters::network.MicrocontrollerIP);
    mcIpAddress.Service(Parameters::network.Port);
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

// Заполняет контрол источника данных списком последовательных портов или сетевым адресом
void FormMain::FillDataSource()
{
    comboBoxDataSource->Clear();

    switch (mode)
    {
    case Modes::Serial:
    {
        auto ports = ThreadedSerialPort::Enumerate();
        wxString serialPortToolTip;
        if (ports.size() > 0)
        {
            for (auto& port : ports)
            {
                comboBoxDataSource->Append(port.Port);
                if (port.HardwareID.IsEmpty())
                {
                    serialPortToolTip += port.Port + TEXT_CR_SPACES + port.Description + TEXT_CR_CR;
                }
                else
                {
                    serialPortToolTip += port.Port + TEXT_CR_SPACES + port.Description + TEXT_CR_SPACES + port.HardwareID + TEXT_CR_CR;
                }
            }
            serialPortToolTip.RemoveLast(2);
            // по умолчанию выбрать первый порт из списка
            if (ports.size() > 0)
            {
                comboBoxDataSource->Select(0);
            }
            comboBoxDataSource->SetToolTip(serialPortToolTip);
        }
        else
        {
            comboBoxDataSource->SetToolTip(TEXT_SERIAL_PORT);
        }
        break;
    }

    case Modes::Network:
    {
        comboBoxDataSource->SetValue(mcIpAddress.OrigHostname());
        comboBoxDataSource->SetToolTip(TEXT_MC_IP_ADDRESS);
        break;
    }
    }
}
