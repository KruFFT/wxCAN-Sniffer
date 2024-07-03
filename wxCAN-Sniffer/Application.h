#pragma once

//#pragma warning(disable : 4996)
//#include <vld.h>

//#include <wx/wxprec.h>
//#ifndef WX_PRECOMP
	#include <wx/wx.h>
//#endif

//  Класс приложения
class Application : public wxApp
{
public:
	bool OnInit();
private:
	wxLocale locale;
};

wxDECLARE_APP(Application);
wxIMPLEMENT_APP(Application);
