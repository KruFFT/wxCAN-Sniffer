#include "Application.h"
#include "FormMain.h"

bool Application::OnInit()
{
	if (locale.Init())
	{
		//locale.AddCatalog(GetAppDir() + wxT("\\."));
	}

	WindowColors colors;

	// определение системной темы, если тёмная
	if (wxSystemSettings::GetAppearance().AreAppsDark())
	{
		colors.IsDark = true;
		colors.GridFont = *wxWHITE;
		colors.GridLines = wxColour(0xFF404040ul);					// ABGR
		colors.GridBackground = *wxBLACK;
		colors.GridNewBackground = wxColour(0xFF008000lu);			// ABGR
		colors.GridUpdateBackground = *wxRED;
		colors.GridSelectedBackground = wxColour(0xFFE3A04Clu);		// ABGR

		colors.GraphFrame = *wxWHITE;
		colors.GraphBackground = *wxBLACK;
		colors.GraphDraw = *wxRED;
		colors.GraphText = *wxWHITE;
	}
	else
	{
		colors.IsDark = false;
		colors.GridFont = *wxBLACK;
		colors.GridLines = wxColour(0xFFC0C0C0ul);					// ABGR
		colors.GridBackground = *wxWHITE;
		colors.GridNewBackground = *wxGREEN;
		colors.GridUpdateBackground = *wxRED;
		colors.GridSelectedBackground = wxColour(0xFFE3A04Clu);		// ABGR

		colors.GraphFrame = *wxBLACK;
		colors.GraphBackground = *wxWHITE;
		colors.GraphDraw = *wxRED;
		colors.GraphText = *wxBLACK;
	}

	auto form = new FormMain(colors);
	form->Show(true);
	SetTopWindow(form);

	return true;
}
