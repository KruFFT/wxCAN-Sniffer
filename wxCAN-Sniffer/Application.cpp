#include "Application.h"

bool Application::OnInit()
{
    if (locale.Init())
    {
        //locale.AddCatalog(GetAppDir() + wxT("\\."));
    }

    // установка названия приложения
    SetVendorName(VENDOR_NAME);
    SetVendorDisplayName(VENDOR_NAME);
    SetAppName(APPLICATION_NAME);
    SetAppDisplayName(APPLICATION_NAME);

    // параметры приложения
    Parameters::Init(SETTINGS_FILE_NAME);

    auto form = new FormMain();
    form->Show(true);
    SetTopWindow(form);

    return true;
}
