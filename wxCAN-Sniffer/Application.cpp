#include "Application.h"
#include "FormMain.h"

bool Application::OnInit()
{
	if (locale.Init())
	{
		//locale.AddCatalog(GetAppDir() + wxT("\\."));
	}

	// определение системной темы, если тёмная
	/*if (wxSystemSettings::GetAppearance().AreAppsDark())
	{

	}
	else
	{

	}*/

	auto form = new FormMain();
	form->Show(true);
	SetTopWindow(form);

	return true;
}
