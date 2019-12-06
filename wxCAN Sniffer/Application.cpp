#include "Application.h"
#include "FormMain.h"

bool Application::OnInit()
{
	if (locale.Init())
	{
		//locale.AddCatalog(GetAppDir() + wxT("\\myapp"));
	}

	FormMain* form = new FormMain();
	form->Show(true);
	SetTopWindow(form);

	return true;
}
