#pragma once

//#pragma warning(disable : 4996)
//#include <vld.h>

#include "Common.h"
#include "FormMain.h"

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
