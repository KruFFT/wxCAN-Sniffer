#include "Application.h"
#include "FormMain.h"

bool Application::OnInit()
{
    if (locale.Init())
    {
        //locale.AddCatalog(GetAppDir() + wxT("\\."));
    }

    auto systemAppearance = wxSystemSettings::GetAppearance();
    WindowColors colors;

    // https://docs.wxwidgets.org/latest/settings_8h.html
    // но почему-то, цвета не определяются в соответствии с выбранной темой

    // назначение общих стилей
    colors.WindowBackground = wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK);
    colors.GridSelectedBackground = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);

    // определение системной темы, если тёмная
    if (wxSystemSettings::GetAppearance().AreAppsDark())
    {
        colors.IsDark = true;
        
        colors.GridFont = *wxWHITE;
        colors.GridLines = wxColour(0xFF404040ul);                  // ABGR
        colors.GridBackground = *wxBLACK;
        colors.GridNewBackground = wxColour(0xFF008000ul);          // ABGR
        colors.GridUpdateBackground = *wxRED;
        
        colors.GraphFrame = *wxWHITE;
        colors.GraphBackground = *wxBLACK;
        colors.GraphDraw = *wxRED;
        colors.GraphText = *wxWHITE;
    }
    else
    {
        colors.IsDark = false;

        colors.GridFont = *wxBLACK;
        colors.GridLines = wxColour(0xFFC0C0C0ul);                  // ABGR
        colors.GridBackground = *wxWHITE;
        colors.GridNewBackground = *wxGREEN;
        colors.GridUpdateBackground = *wxRED;

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
