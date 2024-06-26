﻿#include "FormMain.h"

// События из фонового потока
wxDEFINE_EVENT(wxEVT_SERIAL_PORT_THREAD_UPDATE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_SERIAL_PORT_THREAD_EXIT, wxThreadEvent);

// Талица событий
wxBEGIN_EVENT_TABLE(FormMain, wxFrame)
	EVT_CLOSE(FormMain::OnClose)
	EVT_BUTTON(ID_BUTON_CONNECT_DISCONNECT, FormMain::ButtonConDiscon_OnClick)
	EVT_BUTTON(ID_BUTTON_ADD, FormMain::FormMain::ButtonAdd_OnClick)
	EVT_BUTTON(ID_BUTTON_REMOVE, FormMain::ButtonRemove_OnClick)
	EVT_BUTTON(ID_BUTTON_REMOVE_ALL, FormMain::ButtonRemoveAll_OnClick)
	EVT_BUTTON(ID_BUTTON_SEND, FormMain::ButtonSend_OnClick)
	EVT_BUTTON(ID_BUTTON_CLEAR_LOG, FormMain::ButtonClearCANLog_OnClick)
	EVT_GRID_CMD_SELECT_CELL(ID_GRID_CAN_VIEW, FormMain::GridCANView_OnSelectCell)
	EVT_CHOICE(ID_COMBO_EXT, FormMain::ComboExt_OnChoice)
	EVT_CHOICE(ID_COMBO_SEP, FormMain::ComboSep_OnChoice)
	EVT_CHECKBOX(ID_CHECKBOX_LOG_ENABLE, FormMain::CheckLogEnable_OnClick)
	EVT_CHECKBOX(ID_CHECKBOX_DEC, FormMain::CheckDec_OnClick)
	EVT_CHECKBOX(ID_CHECKBOX_SINGLE, FormMain::CheckSingle_OnClick)
	EVT_CHECKBOX(ID_CHECKBOX_ENDIAN, FormMain::CheckEndian_OnClick)
	EVT_CHECKBOX(ID_CHECKBOX_ASCII, FormMain::CheckASCII_OnClick)
	EVT_TEXT_ENTER(ID_TEXT_DEC_WORD_MUL, FormMain::TextDecWordMul_OnEnter)
	EVT_TEXT_ENTER(ID_TEXT_CAN_ANSWER_ID, FormMain::TextCANAnswerID_OnEnter)
	EVT_TIMER(ID_MAIN_TIMER, FormMain::MainTimer_OnTimer)
	EVT_SOCKET(ID_UDP_SOCKET, FormMain::UDPSocket_OnEvent)
wxEND_EVENT_TABLE()

// Конструктор окна
FormMain::FormMain() : wxFrame(nullptr, ID_MAIN_FORM, CAPTION, wxDefaultPosition, wxSize(940, 640), wxDEFAULT_FRAME_STYLE)
{
	// иконка
	this->SetIcon(wxICON(wxicon));
	this->SetSizeHints(wxSize(1110, 600));

	// главный сайзер окна
	wxBoxSizer* sizerMain = new wxBoxSizer(wxHORIZONTAL);
	{
		splitterLeft = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE | wxSP_3D);

		panelLeftTop = new wxPanel(splitterLeft, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
		panelLeftBottom = new wxPanel(splitterLeft, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);

		// левый верхний сайзер
		wxStaticBoxSizer* sizerLeftTop = new wxStaticBoxSizer(new wxStaticBox(panelLeftTop, wxID_ANY, wxT("Просмотр пакетов")), wxVERTICAL);
		{
			gridCANView = new wxGrid(panelLeftTop, ID_GRID_CAN_VIEW);
			// параметры сетки
			gridCANView->CreateGrid(0, 11);
			gridCANView->EnableEditing(false);
			gridCANView->EnableGridLines(true);
			gridCANView->EnableDragGridSize(false);
			gridCANView->SetMargins(0, 0);
			// параметры столбцов
			gridCANView->EnableDragColMove(false);
			gridCANView->EnableDragColSize(false);
			gridCANView->SetColLabelSize(20);
			gridCANView->SetColLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
			// параметры строк
			gridCANView->EnableDragRowSize(false);
			gridCANView->SetRowLabelSize(40);
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
			for (size_t iCol = 0; iCol < 11; iCol++)
			{
				gridCANView->SetColSize(iCol, 60);
			}
			sizerLeftTop->Add(gridCANView, 1, wxEXPAND, 0);

			panelLeftTop->SetSizer(sizerLeftTop);
		}
		// левый нижний сайзер
		wxStaticBoxSizer* sizerLeftBottom = new wxStaticBoxSizer(new wxStaticBox(panelLeftBottom, wxID_ANY, wxT("Тестовая отправка пакета и просмотр ответа")), wxVERTICAL);
		{
			// сайзер с текстовыми полями ввода данных CAN-пакета
			wxBoxSizer* sizerLeftBottomText = new wxBoxSizer(wxHORIZONTAL);
			{
				wxStaticText* labelCAN = new wxStaticText(panelLeftBottom, wxID_ANY, wxT("CAN:"), wxDefaultPosition, wxSize(37, 20));
				labelCAN->Wrap(-1);
				sizerLeftBottomText->Add(labelCAN, 0, wxLEFT | wxTOP, 2);
				textCANID = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("7E0"), wxDefaultPosition, wxSize(51, 20), wxTE_CENTRE | wxBORDER_SIMPLE);
				sizerLeftBottomText->Add(textCANID, 0, wxEXPAND, 0);
				textCANLength = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("8"), wxDefaultPosition, wxSize(50, 20), wxTE_CENTRE | wxBORDER_SIMPLE);
				sizerLeftBottomText->Add(textCANLength, 0, wxEXPAND, 0);
				textCANByte1 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("05"), wxDefaultPosition, wxSize(50, 20), wxTE_CENTRE | wxBORDER_SIMPLE);
				sizerLeftBottomText->Add(textCANByte1, 0, wxEXPAND, 0);
				textCANByte2 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("2F"), wxDefaultPosition, wxSize(50, 20), wxTE_CENTRE | wxBORDER_SIMPLE);
				sizerLeftBottomText->Add(textCANByte2, 0, wxEXPAND, 0);
				textCANByte3 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("00"), wxDefaultPosition, wxSize(50, 20), wxTE_CENTRE | wxBORDER_SIMPLE);
				sizerLeftBottomText->Add(textCANByte3, 0, wxEXPAND, 0);
				textCANByte4 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("0A"), wxDefaultPosition, wxSize(50, 20), wxTE_CENTRE | wxBORDER_SIMPLE);
				sizerLeftBottomText->Add(textCANByte4, 0, wxEXPAND, 0);
				textCANByte5 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("06"), wxDefaultPosition, wxSize(50, 20), wxTE_CENTRE | wxBORDER_SIMPLE);
				sizerLeftBottomText->Add(textCANByte5, 0, wxEXPAND, 0);
				textCANByte6 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("FF"), wxDefaultPosition, wxSize(50, 20), wxTE_CENTRE | wxBORDER_SIMPLE);
				sizerLeftBottomText->Add(textCANByte6, 0, wxEXPAND, 0);
				textCANByte7 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("00"), wxDefaultPosition, wxSize(50, 20), wxTE_CENTRE | wxBORDER_SIMPLE);
				sizerLeftBottomText->Add(textCANByte7, 0, wxEXPAND, 0);
				textCANByte8 = new wxTextCtrl(panelLeftBottom, wxID_ANY, wxT("00"), wxDefaultPosition, wxSize(50, 20), wxTE_CENTRE | wxBORDER_SIMPLE);
				sizerLeftBottomText->Add(textCANByte8, 0, wxEXPAND, 0);

				sizerLeftBottom->Add(sizerLeftBottomText, 0, wxEXPAND, 0);
			}

			// сайзер с кнопками управления отправкой и журналом ответа
			wxBoxSizer* sizerLeftBottomButtons = new wxBoxSizer(wxHORIZONTAL);
			{
				buttonSend = new wxButton(panelLeftBottom, ID_BUTTON_SEND, wxT("Отправить CAN-пакет"));
				sizerLeftBottomButtons->Add(buttonSend, 0, wxALL, 4);
				wxStaticText* labelCANFromID = new wxStaticText(panelLeftBottom, wxID_ANY, wxT("Отображать ответ от:"), wxDefaultPosition, wxDefaultSize);
				labelCANFromID->Wrap(-1);
				sizerLeftBottomButtons->Add(labelCANFromID, 0, wxALL, 9);
				textCANAnswerID = new wxTextCtrl(panelLeftBottom, ID_TEXT_CAN_ANSWER_ID, wxT("7E8"), wxDefaultPosition, wxSize(51, 20), wxTE_CENTRE | wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
				sizerLeftBottomButtons->Add(textCANAnswerID, 0, wxALL, 7);
				// спейсер между кнопками
				sizerLeftBottomButtons->Add(0, 0, 1, wxEXPAND, 0);
				buttonClearCANLog = new wxButton(panelLeftBottom, ID_BUTTON_CLEAR_LOG, wxT("Очистить ответ"));
				sizerLeftBottomButtons->Add(buttonClearCANLog, 0, wxALL, 4);

				sizerLeftBottom->Add(sizerLeftBottomButtons, 0, wxEXPAND, 0);
			}


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
			gridCANLog->SetColLabelSize(20);
			gridCANLog->SetColLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
			// параметры строк
			gridCANLog->EnableDragRowSize(false);
			gridCANLog->SetRowLabelSize(40);
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
				gridCANLog->SetColSize(iCol, 50);
			}
			sizerLeftBottom->Add(gridCANLog, 1, wxEXPAND, 0);

			panelLeftBottom->SetSizer(sizerLeftBottom);
		}

		splitterLeft->SplitHorizontally(panelLeftTop, panelLeftBottom, -1);
		splitterLeft->SetMinimumPaneSize(30);

		sizerMain->Add(splitterLeft, 1, wxALL | wxEXPAND, 4);

		// правый сайзер
		wxBoxSizer* sizerRight = new wxBoxSizer(wxVERTICAL);
		{
			// COM-порт, кнопка управления и статистика буфера
			wxStaticBoxSizer* sizerControls = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Управление")), wxHORIZONTAL);
			{
				textCOM = new wxTextCtrl(this, wxID_ANY, COM_NAME, wxDefaultPosition, wxSize(50, 22), wxTE_CENTRE | wxBORDER_SIMPLE);
				sizerControls->Add(textCOM, 0, wxALL, 2);

				comboBoxSpeed = new wxComboBox(this, wxID_ANY, wxT("500000"), wxDefaultPosition, wxSize(70, 22), 0, nullptr, wxTE_CENTRE | wxBORDER_SIMPLE);
				comboBoxSpeed->Append(wxT("57600"));
				comboBoxSpeed->Append(wxT("115200"));
				comboBoxSpeed->Append(wxT("250000"));
				comboBoxSpeed->Append(wxT("500000"));
				comboBoxSpeed->Append(wxT("1000000"));
				comboBoxSpeed->Append(wxT("2000000"));
				sizerControls->Add(comboBoxSpeed, 0, wxALL, 2);

				buttonConnectDisconnect = new wxButton(this, ID_BUTON_CONNECT_DISCONNECT, wxT("Подключить"), wxDefaultPosition, wxSize(70, 27));
				sizerControls->Add(buttonConnectDisconnect, 1, wxALL, 0);

				textFPS = new wxTextCtrl(this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize(50, 22), wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
				sizerControls->Add(textFPS, 0, wxALL, 2);

				textBPS = new wxTextCtrl(this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize(50, 22), wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
				sizerControls->Add(textBPS, 0, wxALL, 2);

				sizerRight->Add(sizerControls, 0, wxALL | wxEXPAND, 2);
			}

			// журнал, кнопки управления логом и его параметры
			wxStaticBoxSizer* sizerLog = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Запись в журнал и фильтр")), wxVERTICAL);
			{
				checkLogEnable = new wxCheckBox(this, ID_CHECKBOX_LOG_ENABLE, wxT("Вести запись в журнал"));
				checkLogEnable->SetValue(logEnable);
				sizerLog->Add(checkLogEnable, 0, wxALL, 5);

				wxBoxSizer* sizerLogButtonsList = new wxBoxSizer(wxHORIZONTAL);
				{
					// кнопки журнала
					wxBoxSizer* sizerLogButtons = new wxBoxSizer(wxVERTICAL);
					{
						buttonAdd = new wxButton(this, ID_BUTTON_ADD, wxT("Добавить ID в фильтр >>"), wxDefaultPosition, wxSize(150, -1));
						sizerLogButtons->Add(buttonAdd, 0, wxALL, 2);

						buttonRemove = new wxButton(this, ID_BUTTON_REMOVE, wxT("Убрать ID из фильтра <<"), wxDefaultPosition, wxSize(150, -1));
						sizerLogButtons->Add(buttonRemove, 0, wxALL, 2);

						buttonRemoveAll = new wxButton(this, ID_BUTTON_REMOVE_ALL, wxT("Очистить фильтр"), wxDefaultPosition, wxSize(150, -1));
						sizerLogButtons->Add(buttonRemoveAll, 0, wxALL, 2);

						sizerLogButtonsList->Add(sizerLogButtons, 0, wxEXPAND, 2);
					}
					// список кадров журнала
					listLog = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), 0, nullptr);
					sizerLogButtonsList->Add(listLog, 1, wxALL | wxEXPAND, 2);

					sizerLog->Add(sizerLogButtonsList, 0, wxEXPAND, 2);
				}

				// настройки типа файла журнала
				wxBoxSizer* sizerLogType = new wxBoxSizer(wxHORIZONTAL);
				{
					wxStaticText* labelExt = new wxStaticText(this, wxID_ANY, wxT("Тип файла:"));
					labelExt->Wrap(-1);
					sizerLogType->Add(labelExt, 0, wxALL, 8);

					wxArrayString comboExtChoices;
					comboExtChoices.Add(wxT(".csv"));
					comboExtChoices.Add(wxT(".log"));
					comboExt = new wxChoice(this, ID_COMBO_EXT, wxDefaultPosition, wxDefaultSize, comboExtChoices);
					comboExt->SetSelection(0);
					logExt = wxT(".csv");
					sizerLogType->Add(comboExt, 0, wxALL, 2);

					wxStaticText* labelSep = new wxStaticText(this, wxID_ANY, wxT("Разделитель:"));
					labelSep->Wrap(-1);
					sizerLogType->Add(labelSep, 0, wxALL, 8);

					wxArrayString comboSepChoices;
					comboSepChoices.Add(wxT(";"));
					comboSepChoices.Add(wxT("Tab"));
					comboSepChoices.Add(wxT("-"));
					comboSepChoices.Add(wxT("_"));
					comboSepChoices.Add(wxT("Space"));

					comboSep = new wxChoice(this, ID_COMBO_SEP, wxDefaultPosition, wxDefaultSize, comboSepChoices);
					comboSep->SetSelection(0);
					logSeparator = wxT(";");
					sizerLogType->Add(comboSep, 0, wxALL, 2);

					sizerLog->Add(sizerLogType, 0, wxALIGN_CENTER, 2);
				}

				// параметры сохранения данных в лог
				wxBoxSizer* sizerLogParameters = new wxBoxSizer(wxVERTICAL);
				{
					checkSingle = new wxCheckBox(this, ID_CHECKBOX_SINGLE, wxT("Сохранение данных в один файл"));
					checkSingle->SetValue(logSingle);
					sizerLogParameters->Add(checkSingle, 0, wxALL, 5);
					checkDec = new wxCheckBox(this, ID_CHECKBOX_DEC, wxT("Десятичный вывод данных"));
					checkDec->SetValue(logDecimal);
					sizerLogParameters->Add(checkDec, 0, wxALL, 5);
					checkASCII = new wxCheckBox(this, ID_CHECKBOX_ASCII, wxT("Добавлять ASCII данные"));
					checkASCII->SetValue(logASCII);
					sizerLogParameters->Add(checkASCII, 0, wxALL, 5);

					sizerLog->Add(sizerLogParameters, 0, wxALIGN_LEFT, 2);
				}
				sizerRight->Add(sizerLog, 0, wxALL | wxEXPAND, 2);
			}

			// элементы для наглядного представления чисел
			wxStaticBoxSizer* sizerDecoders = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Декодированные значения")), wxVERTICAL);
			{
				// отображение двоичного байта
				wxBoxSizer* sizerDecoderBinByte = new wxBoxSizer(wxHORIZONTAL);
				{
					wxStaticText* labelBinByte = new wxStaticText(this, wxID_ANY, wxT("Двоичный байт:"), wxDefaultPosition, wxSize(120, -1));
					labelBinByte->Wrap(-1);
					sizerDecoderBinByte->Add(labelBinByte, 0, wxALL | wxEXPAND, 3);
					textBinByte = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(200, 22), wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
					sizerDecoderBinByte->Add(textBinByte, 0, wxALL | wxEXPAND, 0);
					sizerDecoders->Add(sizerDecoderBinByte, 0, wxALIGN_CENTER | wxALL, 2);
				}

				// отображение десятичного байта
				wxBoxSizer* sizerDecoderDecByte = new wxBoxSizer(wxHORIZONTAL);
				{
					wxStaticText* labelDecByte = new wxStaticText(this, wxID_ANY, wxT("Десятичный байт:"), wxDefaultPosition, wxSize(120, -1));
					labelDecByte->Wrap(-1);
					sizerDecoderDecByte->Add(labelDecByte, 0, wxALL | wxEXPAND, 3);
					textDecByte = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(200, 22), wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
					sizerDecoderDecByte->Add(textDecByte, 0, wxALL | wxEXPAND, 0);
					sizerDecoders->Add(sizerDecoderDecByte, 0, wxALIGN_CENTER | wxALL, 2);
				}

				// выбор big endian или little endian
				checkEndian = new wxCheckBox(this, ID_CHECKBOX_ENDIAN, wxT("Последовательность байтов big-endian"));
				checkEndian->SetValue(bigEndian);
				sizerDecoders->Add(checkEndian, 0, wxALL, 5);

				// отображение десятичного слова
				wxBoxSizer* sizerDecoderDecWord = new wxBoxSizer(wxHORIZONTAL);
				{
					wxStaticText* labelDecWord = new wxStaticText(this, wxID_ANY, wxT("Десятичное слово:"), wxDefaultPosition, wxSize(120, -1));
					labelDecWord->Wrap(-1);
					sizerDecoderDecWord->Add(labelDecWord, 0, wxALL | wxEXPAND, 3);
					textDecWord = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(200, 22), wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
					sizerDecoderDecWord->Add(textDecWord, 0, wxALL | wxEXPAND, 0);
					sizerDecoders->Add(sizerDecoderDecWord, 0, wxALIGN_CENTER | wxALL, 2);
				}

				// множитель десятичного слова
				wxBoxSizer* sizerDecoderWordMul = new wxBoxSizer(wxHORIZONTAL);
				{
					wxStaticText* labelDecWordMul = new wxStaticText(this, wxID_ANY, wxT("Множитель слова:"), wxDefaultPosition, wxSize(120, -1));
					labelDecWordMul->Wrap(-1);
					sizerDecoderWordMul->Add(labelDecWordMul, 0, wxALL | wxEXPAND, 3);
					textDecWordMul = new wxTextCtrl(this, ID_TEXT_DEC_WORD_MUL, wxString::Format(wxT("%1.6f"), mul), wxDefaultPosition, wxSize(200, 22), wxTE_CENTRE | wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
					sizerDecoderWordMul->Add(textDecWordMul, 0, wxALL | wxEXPAND, 0);
					sizerDecoders->Add(sizerDecoderWordMul, 0, wxALIGN_CENTER | wxALL, 2);
				}

				// результат умножения десятичного слова
				wxBoxSizer* sizerDecoderDecWordResult = new wxBoxSizer(wxHORIZONTAL);
				{
					wxStaticText* labelDecWordResult = new wxStaticText(this, wxID_ANY, wxT("Результат:"), wxDefaultPosition, wxSize(120, -1));
					labelDecWordResult->Wrap(-1);
					sizerDecoderDecWordResult->Add(labelDecWordResult, 0, wxALL | wxEXPAND, 3);
					textDecWordResult = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, 22), wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
					sizerDecoderDecWordResult->Add(textDecWordResult, 0, wxALL | wxEXPAND, 0);
					sizerDecoders->Add(sizerDecoderDecWordResult, 0, wxALIGN_CENTER | wxALL, 2);
				}

				sizerRight->Add(sizerDecoders, 0, wxALL | wxEXPAND, 2);
			}

			// панель для графика
			drawPanel = new wxPanel(this, ID_DRAW_PANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
			drawPanel->SetBackgroundColour(wxColour(255, 255, 255));
			sizerRight->Add(drawPanel, 1, wxALL | wxEXPAND, 2);

			sizerMain->Add(sizerRight, 0, wxALL | wxEXPAND, 2);
		}
	}

	this->SetSizer(sizerMain);
	this->SetAutoLayout(true);
	this->Layout();
	this->Centre(wxBOTH);
	this->SetDoubleBuffered(true);
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	// привязка событий (так нравится больше, но переделал на таблицу событий в начале кода, так как не смог сделать событие для UDP-сокета)
	//buttonConnectDisconnect->Bind(wxEVT_BUTTON, &FormMain::ButtonConDiscon_OnClick, this);
	//buttonAdd->Bind(wxEVT_BUTTON, &FormMain::ButtonAdd_OnClick, this);
	//buttonRemove->Bind(wxEVT_BUTTON, &FormMain::ButtonRemove_OnClick, this);
	//buttonRemoveAll->Bind(wxEVT_BUTTON, &FormMain::ButtonRemoveAll_OnClick, this);
	//buttonSend->Bind(wxEVT_BUTTON, &FormMain::ButtonSend_OnClick, this);
	//buttonClearCANLog->Bind(wxEVT_BUTTON, &FormMain::ButtonClearCANLog_OnClick, this);
	//gridCANView->Bind(wxEVT_GRID_SELECT_CELL, &FormMain::GridCANView_OnSelectCell, this); 
	//comboExt->Bind(wxEVT_CHOICE, &FormMain::ComboExt_OnChoice, this);
	//comboSep->Bind(wxEVT_CHOICE, &FormMain::ComboSep_OnChoice, this);
	//checkDec->Bind(wxEVT_CHECKBOX, &FormMain::CheckDec_OnClick, this);
	//checkSingle->Bind(wxEVT_CHECKBOX, &FormMain::CheckSingle_OnClick, this);
	//checkEndian->Bind(wxEVT_CHECKBOX, &FormMain::CheckEndian_OnClick, this);
	//checkASCII->Bind(wxEVT_CHECKBOX, &FormMain::CheckASCII_OnClick, this);
	//textDecWordMul->Bind(wxEVT_TEXT_ENTER, &FormMain::TextDecWordMul_OnEnter, this);
	//textCANAnswerID->Bind(wxEVT_TEXT_ENTER, &FormMain::TextCANAnswerID_OnEnter, this);
	//this->Bind(wxEVT_TIMER, &FormMain::MainTimer_OnTimer, this, ID_MAIN_TIMER);

	// эту привязку событий оставил, так как иначе придётся делать наследование от wxPanel
	drawPanel->Bind(wxEVT_PAINT, &FormMain::DrawPanel_OnPaint, this);
	drawPanel->Bind(wxEVT_SIZE, &FormMain::DrawPanel_OnSize, this);
	drawPanel->Bind(wxEVT_ERASE_BACKGROUND, &FormMain::DrawPanel_OnEraseBackground, this);
	drawFrameSize = drawPanel->GetClientSize().GetWidth();
	drawData = new CircularFrameBuffer(drawFrameSize);

	// событие от фонового потока COM-порта, а это не смог нормально описать в таблице событий
	this->Bind(wxEVT_SERIAL_PORT_THREAD_UPDATE, &FormMain::Thread_OnUpdate, this);
	this->Bind(wxEVT_SERIAL_PORT_THREAD_EXIT, &FormMain::Thread_OnExit, this);

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
	UDP = new wxDatagramSocket(ipAddress);

	if (UDP->IsOk())
	{
		UDP->SetEventHandler(*this, ID_UDP_SOCKET);
		UDP->SetNotify(wxSOCKET_INPUT_FLAG);
		UDP->Notify(true);
	}
	else if (UDP->Error())
	{
		wxMessageBox(wxT("Ошибка открытия UDP-сокета: " + UDP->LastError()));
	}
}

// Нажатие кнопки закрытия окна
void FormMain::OnClose(wxCloseEvent& event)
{
	timerMain->Stop();

	if (UDP)
	{
		UDP->Destroy();
		UDP = nullptr;
	}

	if (COM)
	{
		COM->Delete();
		COM = nullptr;
	}

	if (drawData)
	{
		delete drawData;
		drawData = nullptr;
	}

	// записать все файлы
	FlushLogs();

	Destroy();
}

// Обработчик кнопки Подключить/отключить
void FormMain::ButtonConDiscon_OnClick(wxCommandEvent& event)
{
	rowToView = -1;
	colToView = -1;
	drawData->Clear();

	try
	{
		// если порт не открыт - открыть, иначе - закрыть
		if (COM == nullptr)
		{
			DWORD comSpeed = 0;
			if (comboBoxSpeed->GetValue().ToULong(&comSpeed, 10))
			{
				frames.Clear();

				// удалить все строки таблицы
				if (gridCANView->GetNumberRows() > 0)
				{
					gridCANView->DeleteRows(0, gridCANView->GetNumberRows());
				}

				COM = new ThreadedSerialPort(textCOM->GetValue(), comSpeed, (wxFrame*)this);
				buttonConnectDisconnect->SetLabelText(wxT("Отключить"));
			}
		}
		else
		{
			if (COM->hSerial != INVALID_HANDLE_VALUE)
			{
				COM->Delete();
			}
			COM = nullptr;

			buttonConnectDisconnect->SetLabelText(wxT("Подключить"));

			// записать все log-файлы
			FlushLogs();
			logFiles.clear();
		}
	}
	catch (...)
	{
		wxMessageBox(wxT("Невозможно работать с этим COM-портом"));
	}
}

// По событию от потока забирать все принятые CAN-пакеты, которые есть в буфере
void FormMain::Thread_OnUpdate(wxThreadEvent& event)
{
	CANFrame frame;

	if (COM)
	{
		while (COM->GetNextFrame(frame))
		{
			ProcessCANFrame(frame);
		}
	}
}

// По событию от потока забирать все принятые CAN-пакеты, которые есть в буфере
void FormMain::Thread_OnExit(wxThreadEvent& event)
{
	if (COM)
	{
		COM = nullptr;
	}
	buttonConnectDisconnect->SetLabelText(wxT("Подключить"));
}

// Обработка поступившего CAN-пакета
void FormMain::ProcessCANFrame(CANFrame& frame)
{
	bool found = false;

	// если это пакет с адресом 000 - это статистика и её надо вывести отдельно
	if (frame.id == 0 && frame.length >= 4)
	{
		uint16_t fps = ((uint16_t)frame.data[0] << 8) + (uint16_t)frame.data[1];
		textFPS->SetValue(wxString::Format(wxT("%i"), fps));	// кадров в секунду
		uint16_t bps = ((uint16_t)frame.data[2] << 8) + (uint16_t)frame.data[3];
		textBPS->SetValue(wxString::Format(wxT("%i"), bps));	// байтов в секунду
	}
	else
	{
		frames.AddFrame(frame);
		
		// показать числа в разных форматах
		ShowNumbers();

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

			gridCANLog->SetCellValue(lastRow, 0, wxString::Format(wxT("%03X"), frame.id));
			gridCANLog->SetCellValue(lastRow, 1, wxString::Format(wxT("%i"), frame.length));

			// заполнение столбцов параметров
			for (size_t iData = 0; iData < 8; iData++)
			{
				if (iData < frame.length)
				{
					// вывод данных
					gridCANLog->SetCellValue(lastRow, iData + 2, wxString::Format(wxT("%02X"), frame.data[iData]));
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

// Отображение выделенной ячейки в разных форматах
void FormMain::ShowNumbers()
{
	if (rowToView >= 0 && colToView >= 0)
	{
		// если полученных данных ещё нет
		if (rowToView >= frames.Size())
		{
			return;
		}

		VisualCANFrame vFrame = frames.GetFrame(rowToView);
		uint8_t firstByte = vFrame.frame.data[colToView];
		uint8_t secondByte = colToView < 7 ? vFrame.frame.data[colToView + 1] : 0;

		// выбор между big endian и little endian
		uint32_t value = bigEndian ? (firstByte << 8) + secondByte : (secondByte << 8) + firstByte;

		textDecWord->SetValue(wxString::Format(wxT("%i"), value));
		uint32_t mulValue = (uint32_t)((double)value * mul);
		textDecWordResult->SetValue(wxString::Format(wxT("%i"), mulValue));

		textBinByte->SetValue(ToBinary(firstByte));
		textDecByte->SetValue(wxString::Format(wxT("%i"), firstByte));

		// добавить полученные данные в очередь на отрисовку
		if (drawData && colToView >= 0)
		{
			drawData->Add(mulValue);
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
		listLog->Append(wxString::Format(wxT("%03X"), logFilterIDs[iID]));
	}
}

// При нажатии Enter в поле ввода множителя - обработать его и запомнить
void FormMain::TextDecWordMul_OnEnter(wxCommandEvent& event)
{
	textDecWordMul->GetValue().ToDouble(&mul);

	if (mul == 0)
		mul = 1.0;
	else if (mul > 10000)
		mul = 10000.0;

	textDecWordMul->SetValue(wxString::Format(wxT("%1.6f"), mul));
	drawMaxValue = 0;
}

// Добавить ID в список фильра для записи в log
void FormMain::ButtonAdd_OnClick(wxCommandEvent& event)
{
	if (rowToLog >= 0 && (int)frames.Size() > rowToLog)
	{
		bool found = false;
		for (size_t iID = 0; iID < logFilterIDs.size(); iID++)
		{
			if (frames.GetFrame(rowToLog).frame.id == logFilterIDs[iID])
			{
				found = true;
				break;
			}
		}
		if (!found)
			logFilterIDs.push_back(frames.GetFrame(rowToLog).frame.id);
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
void FormMain::ComboExt_OnChoice(wxCommandEvent& event)
{
	logExt = event.GetString();
}

// Выбор разделителя в log-файле
void FormMain::ComboSep_OnChoice(wxCommandEvent& event)
{
	wxString value = event.GetString();
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
void FormMain::SaveToLog(CANFrame& frame)
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
					wxString logPath = wxGetCwd() + wxT("\\CAN") + logExt;

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
					wxString logPath = wxGetCwd() + wxT("\\CAN ID ") + wxString::Format(wxT("%03X"), frame.id) + logExt;

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
void FormMain::LogWriteLine(wxFFile* file, CANFrame& frame)
{
	// штамп времени
	wxDateTime dtNow = wxDateTime::UNow();
	//wxString newLine = dtNow.FormatDate() + separator + dtNow.FormatTime() + wxT(".") + wxString::Format(wxT("%03i"), dtNow.GetMillisecond()) + separator;
	wxString newLine = dtNow.FormatTime() + decimalSeparator + wxString::Format(wxT("%03i"), dtNow.GetMillisecond()) + logSeparator;

	try
	{
		// идентификатор пакета и его длина
		newLine += wxString::Format(wxT("%03X"), frame.id) + logSeparator;
		newLine += wxString::Format(wxT("%i"), frame.length) + logSeparator;
		// данные пакета
		for (size_t iData = 0; iData < frame.length; iData++)
		{
			if (logDecimal)
			{
				// десятичный вывод
				newLine += wxString::Format(wxT("%0i"), frame.data[iData]) + logSeparator;
			}
			else
			{
				// шестнадцатиричный вывод
				newLine += wxString::Format(wxT("%02X"), frame.data[iData]) + logSeparator;
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
	colToView = event.GetCol() - 2;
	// если выбран столбик не с данными
	if (colToView < 0)
	{
		rowToView = -1;
		colToView = -1;

		textBinByte->SetValue(wxT(""));
		textDecByte->SetValue(wxT(""));
		textDecWord->SetValue(wxT(""));
	}
	else
	{
		if (drawData)
		{
			drawData->Clear();
			drawDataBegin = 0;
			drawMaxValue = 0;
		}
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
		if (value & 0x80)	binaryString += wxT('1');
		else				binaryString += wxT('0');
		value <<= 1;
	}
	binaryString += wxT('_');
	for (size_t counter = 0; counter < 4; counter++)
	{
		if (value & 0x80)	binaryString += wxT('1');
		else				binaryString += wxT('0');
		value <<= 1;
	}
	return binaryString;
}

// Срабатывание таймера
void FormMain::MainTimer_OnTimer(wxTimerEvent& event)
{
	// обновить данные в таблице
	RefreshGridCANView();
	// это вызовет событие OnPaint для панели
	drawPanel->Refresh(true, &drawRect);
}

// Обновить данные CAN-пакетов в таблице, вызывается по таймеру
void FormMain::RefreshGridCANView()
{
	size_t framesCount = frames.Size();

	// заполнить таблицу строками
	while (gridCANView->GetNumberRows() < framesCount)
	{
		gridCANView->InsertRows(0);
	}

	for (size_t iFrame = 0; iFrame < framesCount; iFrame++)
	{
		// вывод ID, интервала и длины пакета
		VisualCANFrame vFrame = frames.GetFrame(iFrame);
		gridCANView->SetCellValue(iFrame, 0, wxString::Format(wxT("%03X"), vFrame.frame.id));
		gridCANView->SetCellValue(iFrame, 1, wxString::Format(wxT("%i"), vFrame.frame.interval));
		gridCANView->SetCellValue(iFrame, 2, wxString::Format(wxT("%i"), vFrame.frame.length));

		// заполнение столбцов данных
		for (size_t iData = 0; iData < 8; iData++)
		{
			if (iData < vFrame.frame.length)
			{
				// вывод данных
				gridCANView->SetCellValue(iFrame, iData + 3, wxString::Format(wxT("%02X"), vFrame.frame.data[iData]));
				gridCANView->SetCellBackgroundColour(iFrame, iData + 3, vFrame.color[iData]);
			}
			else
			{
				// вывод пустых ячеек
				gridCANView->SetCellValue(iFrame, iData + 3, wxT(" "));
				gridCANView->SetCellBackgroundColour(iFrame, iData + 3, wxColor(DEFAULT_COLOR));
			}
			gridCANView->RefreshBlock(iFrame, 3, iFrame, 9);
		}
	}
}

// Событие отрисовки в панели графика
void FormMain::DrawPanel_OnPaint(wxPaintEvent& event)
{
	//wxPaintDC dc(drawPanel);
	wxBufferedPaintDC dc(drawPanel);	// с буферной отрисовкой не мерцает
	
	PrepareDC(dc);

	// нарисовать рамку и фон
	dc.SetPen(blackPen);
	dc.DrawRectangle(drawRect);

	if (drawData && colToView >= 0)
	{
		double drawMul = (drawMaxValue != 0) ? (double)drawRect.height / drawMaxValue : 1.0;

		dc.SetPen(graphPen);

		uint32_t* frame = drawData->Frame();

		for (size_t index = 1; index < drawFrameSize; index++)
		{
			// рисовать отмасштабированную линию
			dc.DrawLine(index, drawRect.height - *(frame + index - 1) * drawMul,
						index, drawRect.height - *(frame + index)     * drawMul);

			// сохранение наибольшего значения для следующей итерации
			if (*(frame + index) > drawMaxValue)
			{
				drawMaxValue = *(frame + index);
			}
		}
	}
}

// Изменение размеров панели графика
void FormMain::DrawPanel_OnSize(wxSizeEvent& event)
{
	drawRect = drawPanel->GetClientRect();
	this->Refresh(true, &drawRect);
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
	CANFrame frame = { 0 };

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
	if (COM)
	{
		COM->SendFrame(frame);
	}
	if (UDP)
	{
		UDPSocket_SendFrame(frame);
	}
}

// Нажатие Enter при вводе номера CAN ID ожидания ответа
void FormMain::TextCANAnswerID_OnEnter(wxCommandEvent& event)
{
	textCANAnswerID->GetValue().ToCULong((unsigned long*)&answerID, 16);
	textCANAnswerID->SetValue(wxString::Format(wxT("%03X"), answerID));
}

// Событие UDP-сокета
void FormMain::UDPSocket_OnEvent(wxSocketEvent& event)
{
	uint8_t  receivedData[UDP_BUFFER_SIZE] = { 0 };
	uint8_t* receivedDataPointer = receivedData;

	if (event.GetSocketEvent() == wxSOCKET_INPUT)
	{
		size_t receivedDataLen = UDP->RecvFrom(espIpAddress, receivedData, UDP_BUFFER_SIZE).LastCount();
		if (receivedDataLen)
		{
			CANFrame frame;
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
void FormMain::UDPSocket_SendFrame(CANFrame& frame)
{
	SendCANFrame sendCANFrame;
	sendCANFrame.Frame = frame;
	UDP->SendTo(espIpAddress, &sendCANFrame, sizeof(SendCANFrame));
}
