#pragma once

#include "Common.h"
#include "ThreadedSerialPort.h"

#define DRAW_DATA_RESERV	1000

#define NEW_COLOR			0x00FF00	// green (BGR)
#define MARKED_COLOR		0x0000FF	// red (BGR)
#define DEFAULT_COLOR		0xFFFFFF	// white (BGR)
#define DRAW_COLOR			0x0000FF	// red (BGR)

#define TIMER_INTERVAL		25			// интервал срабатывания таймера

// Идентификаторы необходимых объектов
enum IDs
{
	idMAINTIMER				= wxID_HIGHEST + 1,
	idTEXTDECWORDMUL,
	idTEXTCANANSWERID
};

//  Класс окна
class FormMain : public wxFrame
{
public:
	FormMain();
	~FormMain();

	// обработчики событий
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

private:
	wxSplitterWindow*	splitterLeft;
	wxPanel*		panelLeftTop;
	wxPanel*		panelLeftBottom;
	wxGrid*			gridCANView;
	wxGrid*         gridCANLog;
	wxTextCtrl*		textCANID;
	wxTextCtrl*		textCANLength;
	wxTextCtrl*		textCANByte1;
	wxTextCtrl*		textCANByte2;
	wxTextCtrl*		textCANByte3;
	wxTextCtrl*		textCANByte4;
	wxTextCtrl*		textCANByte5;
	wxTextCtrl*		textCANByte6;
	wxTextCtrl*		textCANByte7;
	wxTextCtrl*		textCANByte8;
	wxButton*		buttonSend;
	wxTextCtrl*     textCANAnswerID;
	wxButton*		buttonClearCANLog;
	wxTextCtrl*		textCOM;
	wxComboBox*     comboBoxSpeed;
	wxTextCtrl*		textFPS;
	wxTextCtrl*		textBPS;
	wxButton*		buttonConnectDisconnect;
	wxButton*		buttonAdd;
	wxButton*		buttonRemove;
	wxButton*		buttonRemoveAll;
	wxListBox*		listLog;
	wxChoice*		comboExt;
	wxChoice*		comboSep;
	wxCheckBox*		checkDec;
	wxCheckBox*     checkSingle;
	wxCheckBox*     checkASCII;
	wxTextCtrl*		textBinByte;
	wxTextCtrl*		textDecByte;
	wxCheckBox*     checkEndian;
	wxTextCtrl*		textDecWord;
	wxTextCtrl*		textDecWordMul;
	wxTextCtrl*		textDecWordResult;
	wxPanel*		drawPanel;
	wxTimer*		timerMain;

	ThreadedSerialPort* COM = NULL;		// последовательный порт в отдельном потоке
	vector<CANFrame> frames;			// список отображаемых на экране пакетов

	vector<int32_t> logFilterIDs;		// список ID для записи в log-файла
	int32_t rowToLog = -1;				// выбранная в таблице строка для добавления в фильтр log-файла
	wxString logExt;					// расширение log-файла
	wxString logSeparator;				// разделитель значений в log-файле
	bool logDecimal = false;			// переключатель режима записи чисел в log-файле: десятичный или шестнадцатиричный
	bool logSingle = true;				// переключатель режима записи в log-файле: в один или в раздельные
	bool logASCII = false;				// добавлять или нет в log-файле текстовые данные
	
	int32_t rowToView = -1;				// номер строки выбранной ячейки для отображения данных о ней
	int32_t colToView = -1;				// номер столбца выбранной ячейки для отображения данных о ней
	double mul = 0.125;

	vector<LogFile> logFiles;			// массив log-файлов
	wxFFile*        logFile = NULL;		// единый log-файл

	wxString decimalSeparator;			// выбранный символ разделитель для данных в log-файле

	wxRect drawRect;
	vector<uint32_t> drawData;			// массив данных для отрисовки
	uint32_t drawDataBegin = 0;			// начало данных в массиве
	uint32_t drawMaxValue = 0;			// наибольшее отрисовываемое значение дл¤ масштабирования графика
	uint32_t answerID = 0x7E8;			// ID пакета, от которого будут отображатьс¤ данные
	uint32_t timerCounter = 0;

	void ProcessCANFrame(CANFrame* Frame);
	void RefreshListLog();
	void SaveToLog(CANFrame* Frame);
	void FlushLogs();
	void LogWriteLine(wxFFile* File, CANFrame* Frame);
	bool bigEndian = true;
	wxString ToBinary(uint8_t Value);
	void ShowNumbers();
};
