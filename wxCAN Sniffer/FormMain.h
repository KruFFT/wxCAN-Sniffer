#pragma once

#include "Common.h"
#include "ThreadedSerialPort.h"
#include "CircularFrameBuffer.h"

#define FRAMES_DATA_RESERV	100			// колчиество элементов для резерв в векторах

#define NEW_COLOR			0x00FF00	// green (BGR)
#define MARKED_COLOR		0x0000FF	// red (BGR)
#define DEFAULT_COLOR		0xFFFFFF	// white (BGR)
#define DRAW_COLOR			0x0000FF	// red (BGR)

#define TIMER_INTERVAL		10			// интервал срабатывания таймера (около 50 кадров/с)

#define COM_NAME			wxT("COM3")	// последовательный порт по умолчанию
#define UDP_PORT			0xAA55		// UDP порт

// Идентификаторы необходимых объектов
enum IDs
{
	ID_MAIN_FORM = wxID_HIGHEST + 1,
	ID_MAIN_TIMER,
	ID_BUTON_CONNECT_DISCONNECT,
	ID_BUTTON_ADD,
	ID_BUTTON_REMOVE,
	ID_BUTTON_REMOVE_ALL,
	ID_BUTTON_SEND,
	ID_BUTTON_CLEAR_LOG,
	ID_GRID_CAN_VIEW,
	ID_COMBO_EXT,
	ID_COMBO_SEP,
	ID_CHECKBOX_SINGLE,
	ID_CHECKBOX_DEC,
	ID_CHECKBOX_ENDIAN,
	ID_CHECKBOX_ASCII,
	ID_DRAW_PANEL,
	ID_TEXT_DEC_WORD_MUL,
	ID_TEXT_CAN_ANSWER_ID,
	ID_UDP_SOCKET
};

//  Класс окна
class FormMain : public wxFrame
{
public:
	FormMain();

	// обработчики событий
	void OnClose(wxCloseEvent& event);
	void ButtonConDiscon_OnClick(wxCommandEvent& event);
	void ButtonAdd_OnClick(wxCommandEvent& event);
	void ButtonRemove_OnClick(wxCommandEvent& event);
	void ButtonRemoveAll_OnClick(wxCommandEvent& event);
	void ButtonSend_OnClick(wxCommandEvent& event);
	void ButtonClearCANLog_OnClick(wxCommandEvent& event);
	void ComboExt_OnChoice(wxCommandEvent& event);
	void ComboSep_OnChoice(wxCommandEvent& event);
	void CheckDec_OnClick(wxCommandEvent& event);
	void CheckSingle_OnClick(wxCommandEvent& event);
	void CheckASCII_OnClick(wxCommandEvent& event);
	void MainTimer_OnTimer(wxTimerEvent& event);
	void GridCANView_OnSelectCell(wxGridEvent& event);
	void CheckEndian_OnClick(wxCommandEvent& event);
	void TextDecWordMul_OnEnter(wxCommandEvent& event);
	void TextCANAnswerID_OnEnter(wxCommandEvent& event);
	void DrawPanel_OnSize(wxSizeEvent& event);
	void DrawPanel_OnPaint(wxPaintEvent& event);
	void DrawPanel_OnEraseBackground(wxEraseEvent& event);
	void Thread_OnUpdate(wxThreadEvent& event);
	void UDPSocket_OnEvent(wxSocketEvent& event);

private:
	wxSplitterWindow* splitterLeft;
	wxPanel* panelLeftTop;
	wxPanel* panelLeftBottom;
	wxGrid* gridCANView;
	wxGrid* gridCANLog;
	wxTextCtrl* textCANID;
	wxTextCtrl* textCANLength;
	wxTextCtrl* textCANByte1;
	wxTextCtrl* textCANByte2;
	wxTextCtrl* textCANByte3;
	wxTextCtrl* textCANByte4;
	wxTextCtrl* textCANByte5;
	wxTextCtrl* textCANByte6;
	wxTextCtrl* textCANByte7;
	wxTextCtrl* textCANByte8;
	wxButton* buttonSend;
	wxTextCtrl* textCANAnswerID;
	wxButton* buttonClearCANLog;
	wxTextCtrl* textCOM;
	wxComboBox* comboBoxSpeed;
	wxTextCtrl* textFPS;
	wxTextCtrl* textBPS;
	wxButton* buttonConnectDisconnect;
	wxButton* buttonAdd;
	wxButton* buttonRemove;
	wxButton* buttonRemoveAll;
	wxListBox* listLog;
	wxChoice* comboExt;
	wxChoice* comboSep;
	wxCheckBox* checkDec;
	wxCheckBox* checkSingle;
	wxCheckBox* checkASCII;
	wxTextCtrl* textBinByte;
	wxTextCtrl* textDecByte;
	wxCheckBox* checkEndian;
	wxTextCtrl* textDecWord;
	wxTextCtrl* textDecWordMul;
	wxTextCtrl* textDecWordResult;
	wxPanel* drawPanel;
	wxTimer* timerMain;

	ThreadedSerialPort* COM = nullptr;	// последовательный порт в отдельном потоке
	vector<CANFrame> frames;			// список отображаемых на экране пакетов

	vector<int32_t> logFilterIDs;		// список ID для записи в log-файл
	size_t rowToLog = -1;				// выбранная в таблице строка для добавления в фильтр log-файла
	wxString logExt;					// расширение log-файла
	wxString logSeparator;				// разделитель значений в log-файле
	bool logDecimal = false;			// переключатель режима записи чисел в log-файле: десятичный или шестнадцатиричный
	bool logSingle = true;				// переключатель режима записи в log-файле: в один или в раздельные
	bool logASCII = false;				// добавлять или нет в log-файле текстовые данные

	int32_t rowToView = -1;				// номер строки выбранной ячейки для отображения данных о ней
	int32_t colToView = -1;				// номер столбца выбранной ячейки для отображения данных о ней
	double mul = 0.125;

	vector<LogFile> logFiles;			// массив log-файлов
	wxFFile* logFile = nullptr;			// единый log-файл

	wxString decimalSeparator;			// выбранный символ разделитель для данных в log-файле

	wxPen blackPen = *wxBLACK;			// кисть рамки для отрисовки графика
	wxPen graphPen = wxPen(wxColour(DRAW_COLOR, 0, 0), 3);	// кисть для отрисовки графика заданной ширины
	wxRect drawRect;					// область отрисовки графика
	CircularFrameBuffer* drawData = nullptr;	// круговой массив данных для отрисовки
	size_t drawFrameSize;				// размер кадра отрисовки (равен ширине области панель)
	uint32_t drawDataBegin = 0;			// начало данных в массиве
	uint32_t drawMaxValue = 0;			// наибольшее отрисовываемое значение для масштабирования графика
	
	uint32_t answerID = 0x7E8;			// ID пакета, от которого будут отображаться данные
	uint32_t timerCounter = 0;
	bool bigEndian = true;

	wxDatagramSocket* udpSocket;		// UDP-сокет
	wxIPV4address espIpAddress;			// запомненый адрес ESP8266

	void ProcessCANFrame(CANFrame& frame);
	void RefreshListLog();
	void SaveToLog(CANFrame& frame);
	void FlushLogs();
	void LogWriteLine(wxFFile* file, CANFrame& frame);
	wxString ToBinary(uint8_t value);
	void ShowNumbers();

	wxDECLARE_EVENT_TABLE();
};
