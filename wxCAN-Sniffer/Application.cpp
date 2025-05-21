#include "Application.h"

bool Application::OnInit()
{
    if (locale.Init())
    {
        //locale.AddCatalog(GetAppDir() + wxT("\\."));
    }

    #ifdef __WXMAC__
        wxApp::SetExitOnFrameDelete(false);

        wxMenuBar *menubar = new wxMenuBar;
        wxMenuBar::MacSetCommonMenuBar(menubar);
        wxMenu *menu = new wxMenu();
        auto item = menu->Append(ID_MENU_FILE_NEW_WINDOW, MENU_FILE_NEW_WINDOW);
        menu->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {
            if (event.GetId() == ID_MENU_FILE_NEW_WINDOW) {
                ShowNewMainWindow();
            }
        }, wxID_ANY);
        menubar->Append(menu, MENU_FILE);
    #endif

    // установка названия приложения
    SetVendorName(VENDOR_NAME);
    SetVendorDisplayName(VENDOR_NAME);
    SetAppName(APPLICATION_NAME);
    SetAppDisplayName(APPLICATION_NAME);

    // параметры приложения
    Parameters::Init(SETTINGS_FILE_NAME);

    ShowNewMainWindow();

    return true;
}

void Application::ShowNewMainWindow()
{
    auto form = new FormMain();
    form->Show(true);
    SetTopWindow(form);
}
