#include "Parameters.h"

void Parameters::Init(const wxString& iniFile)
{
    wxFileConfig fileConfig(APPLICATION_NAME, VENDOR_NAME, iniFile, iniFile, wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_RELATIVE_PATH);
    fileConfig.EnableAutoSave();

    appearance.ControlsCustomColors = ReadBoolean(fileConfig, PARAMETER_CONTROLS_CUSTOM_COLORS, PARAMETER_CONTROLS_CUSTOM_COLORS_DEFAULT);
    can.Signature = SWAP_BYTES_UINT32(ReadNumber(fileConfig, PARAMETER_CAN_SIGNATURE_DWORD, PARAMETER_CAN_SIGNATURE_DWORD_DEFAULT, UINT32_MAX, true));
    can.ServiceID = ReadNumber(fileConfig, PARAMETER_CAN_SERVICE_ID, PARAMETER_CAN_SERVICE_ID_DEFAULT, UINT32_MAX, true);
    can.MinimalDataSize = ReadNumber(fileConfig, PARAMETER_CAN_MIN_DATA_SIZE, PARAMETER_CAN_MIN_DATA_SIZE_DEFAULT, UINT8_MAX);
    serial.PortSpeed = ReadNumber(fileConfig, PARAMETER_SERIAL_PORT_SPEED, PARAMETER_SERIAL_PORT_SPEED_DEFAULT, UINT32_MAX);
    network.MicrocontrollerIP = ReadString(fileConfig, PARAMETER_MICROCONTROLLER_IP, PARAMETER_MICROCONTROLLER_IP_DEFAULT);
    network.Port = ReadNumber(fileConfig, PARAMETER_PORT, PARAMETER_PORT_DEFAULT, UINT16_MAX);

    // https://docs.wxwidgets.org/latest/settings_8h.html
    // но почему-то, системные цвета стиля не определяются в соответствии с выбранной темой

    // определение цветовой темы приложения
    appearance.isDark = wxSystemSettings::GetAppearance().AreAppsDark();
    if (!appearance.isDark)
    {
        colors.WindowBackground = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_COLOR_WINDOW_BACKGROUND, PARAMETER_COLOR_WINDOW_BACKGROUND_DEFAULT, UINT32_MAX, true));
        colors.ControlText = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_COLOR_CONTROL_TEXT, PARAMETER_COLOR_CONTROL_TEXT_DEFAULT, UINT32_MAX, true));
        colors.ControlBackground = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_COLOR_CONTROL_BACKGROUND, PARAMETER_COLOR_CONTROL_BACKGROUND_DEFAULT, UINT32_MAX, true));

        colors.GridSelectedBackground = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_COLOR_GRID_SELECTED_BACKGROUND, PARAMETER_COLOR_GRID_SELECTED_BACKGROUND_DEFAULT, UINT32_MAX, true));
        colors.GridText = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_COLOR_GRID_TEXT, PARAMETER_COLOR_GRID_TEXT_DEFAULT, UINT32_MAX, true));
        colors.GridLines = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_COLOR_GRID_LINES, PARAMETER_COLOR_GRID_LINES_DEFAULT, UINT32_MAX, true));
        colors.GridBackground = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_COLOR_GRID_BACKGROUND, PARAMETER_COLOR_GRID_BACKGROUND_DEFAULT, UINT32_MAX, true));
        colors.GridNewBackground = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_COLOR_GRID_NEW_BACKGROUND, PARAMETER_COLOR_GRID_NEW_BACKGROUND_DEFAULT, UINT32_MAX, true));
        colors.GridUpdatedBackground = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_COLOR_GRID_UPDATED_BACKGROUND, PARAMETER_COLOR_GRID_UPDATED_BACKGROUND_DEFAULT, UINT32_MAX, true));

        colors.GraphFrame = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_COLOR_GRAPH_FRAME, PARAMETER_COLOR_GRAPH_FRAME_DEFAULT, UINT32_MAX, true));
        colors.GraphBackground = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_COLOR_GRAPH_BACKGROUND, PARAMETER_COLOR_GRAPH_BACKGROUND_DEFAULT, UINT32_MAX, true));
        colors.GraphDraw = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_COLOR_GRAPH_DRAW, PARAMETER_COLOR_GRAPH_DRAW_DEFAULT, UINT32_MAX, true));
        colors.GraphText = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_COLOR_GRAPH_TEXT, PARAMETER_COLOR_GRAPH_TEXT_DEFAULT, UINT32_MAX, true));
    }
    else
    {
        colors.WindowBackground = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_DARK_COLOR_WINDOW_BACKGROUND, PARAMETER_DARK_COLOR_WINDOW_BACKGROUND_DEFAULT, UINT32_MAX, true));
        colors.ControlText = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_DARK_COLOR_CONTROL_TEXT, PARAMETER_DARK_COLOR_CONTROL_TEXT_DEFAULT, UINT32_MAX, true));
        colors.ControlBackground = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_DARK_COLOR_CONTROL_BACKGROUND, PARAMETER_DARK_COLOR_CONTROL_BACKGROUND_DEFAULT, UINT32_MAX, true));

        colors.GridSelectedBackground = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_DARK_COLOR_GRID_SELECTED_BACKGROUND, PARAMETER_DARK_COLOR_GRID_SELECTED_BACKGROUND_DEFAULT, UINT32_MAX, true));
        colors.GridText = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_DARK_COLOR_GRID_TEXT, PARAMETER_DARK_COLOR_GRID_TEXT_DEFAULT, UINT32_MAX, true));
        colors.GridLines = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_DARK_COLOR_GRID_LINES, PARAMETER_DARK_COLOR_GRID_LINES_DEFAULT, UINT32_MAX, true));
        colors.GridBackground = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_DARK_COLOR_GRID_BACKGROUND, PARAMETER_DARK_COLOR_GRID_BACKGROUND_DEFAULT, UINT32_MAX, true));
        colors.GridNewBackground = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_DARK_COLOR_GRID_NEW_BACKGROUND, PARAMETER_DARK_COLOR_GRID_NEW_BACKGROUND_DEFAULT, UINT32_MAX, true));
        colors.GridUpdatedBackground = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_DARK_COLOR_GRID_UPDATED_BACKGROUND, PARAMETER_DARK_COLOR_GRID_UPDATED_BACKGROUND_DEFAULT, UINT32_MAX, true));

        colors.GraphFrame = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_DARK_COLOR_GRAPH_FRAME, PARAMETER_DARK_COLOR_GRAPH_FRAME_DEFAULT, UINT32_MAX, true));
        colors.GraphBackground = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_DARK_COLOR_GRAPH_BACKGROUND, PARAMETER_DARK_COLOR_GRAPH_BACKGROUND_DEFAULT, UINT32_MAX, true));
        colors.GraphDraw = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_DARK_COLOR_GRAPH_DRAW, PARAMETER_DARK_COLOR_GRAPH_DRAW_DEFAULT, UINT32_MAX, true));
        colors.GraphText = SWAP_BYTES_UINT24(ReadNumber(fileConfig, PARAMETER_DARK_COLOR_GRAPH_TEXT, PARAMETER_DARK_COLOR_GRAPH_TEXT_DEFAULT, UINT32_MAX, true));
    }
}

uint32_t Parameters::ReadNumber(wxFileConfig& fileConfig, wxString name, uint32_t defaultValue, uint32_t maxValue, bool hex)
{
    wxString stringValue;
    uint32_t value = defaultValue;

    if (!fileConfig.Read(name, &stringValue) ||
        !stringValue.ToUInt(&value, hex ? 16 : 10) ||
        value > maxValue)
    {
        stringValue = hex ? wxT("0x") + wxString::Format(FORMAT_HEX8, value) : wxString::Format(FORMAT_UINT, value);
        fileConfig.Write(name, stringValue);
    }

    return value;
}

wxString Parameters::ReadString(wxFileConfig& fileConfig, wxString name, wxString defaultValue)
{
    wxString stringValue;
    if (!fileConfig.Read(name, &stringValue, defaultValue))
    {
        fileConfig.Write(name, stringValue);
    }
    return stringValue;
}

bool Parameters::ReadBoolean(wxFileConfig& fileConfig, wxString name, bool defaultValue)
{
    bool booleanValue;
    if (!fileConfig.Read(name, &booleanValue, defaultValue))
    {
        fileConfig.Write(name, booleanValue);
    }
    return booleanValue;
}
