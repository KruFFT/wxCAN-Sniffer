#pragma once

#include "Common.h"
#include <wx/fileconf.h>

#define PARAMETER_CAN_SIGNATURE_DWORD           wxT("CAN/Signature")
#define PARAMETER_CAN_SIGNATURE_DWORD_DEFAULT   0xAA55AA55u
#define PARAMETER_CAN_SERVICE_ID                wxT("CAN/ServiceID")
#define PARAMETER_CAN_SERVICE_ID_DEFAULT        0x00000000u
#define PARAMETER_CAN_MIN_DATA_SIZE             wxT("CAN/MinimalDataSize")
#define PARAMETER_CAN_MIN_DATA_SIZE_DEFAULT     19
#define PARAMETER_SERIAL_PORT_SPEED             wxT("SerialPort/Speed")
#define PARAMETER_SERIAL_PORT_SPEED_DEFAULT     500000
#define PARAMETER_UDP_PORT                      wxT("Network/UdpPort")
#define PARAMETER_UDP_PORT_DEFAULT              0xAA55
#define PARAMETER_UDP_BUFFER_SIZE               wxT("Network/UdpBufferSize")
#define PARAMETER_UDP_BUFFER_SIZE_DEFAULT       1000

#define PARAMETER_COLOR_WINDOW_BACKGROUND                       wxT("Colors/WindowBackground")
#define PARAMETER_COLOR_WINDOW_BACKGROUND_DEFAULT               0xF3F3F3u
#define PARAMETER_COLOR_CONTROL_TEXT                            wxT("Colors/ControlText")
#define PARAMETER_COLOR_CONTROL_TEXT_DEFAULT                    0x1A1A1Au
#define PARAMETER_COLOR_CONTROL_BACKGROUND                      wxT("Colors/ControlBackground")
#define PARAMETER_COLOR_CONTROL_BACKGROUND_DEFAULT              0xFBFBFBu
#define PARAMETER_COLOR_GRID_SELECTED_BACKGROUND                wxT("Colors/GridSelectedBackground")
#define PARAMETER_COLOR_GRID_SELECTED_BACKGROUND_DEFAULT        0x0078D4u
#define PARAMETER_COLOR_GRID_TEXT                               wxT("Colors/GridText")
#define PARAMETER_COLOR_GRID_TEXT_DEFAULT                       0x1A1A1Au
#define PARAMETER_COLOR_GRID_LINES                              wxT("Colors/GridLines")
#define PARAMETER_COLOR_GRID_LINES_DEFAULT                      0xE5E5E5u
#define PARAMETER_COLOR_GRID_BACKGROUND                         wxT("Colors/GridBackground")
#define PARAMETER_COLOR_GRID_BACKGROUND_DEFAULT                 0xFBFBFBu
#define PARAMETER_COLOR_GRID_NEW_BACKGROUND                     wxT("Colors/GridNewBackground")
#define PARAMETER_COLOR_GRID_NEW_BACKGROUND_DEFAULT             0x00CC6Au
#define PARAMETER_COLOR_GRID_UPDATED_BACKGROUND                 wxT("Colors/GridUpdatedBackground")
#define PARAMETER_COLOR_GRID_UPDATED_BACKGROUND_DEFAULT         0xFF4343u
#define PARAMETER_COLOR_GRAPH_FRAME                             wxT("Colors/GraphFrame")
#define PARAMETER_COLOR_GRAPH_FRAME_DEFAULT                     0xE5E5E5u
#define PARAMETER_COLOR_GRAPH_BACKGROUND                        wxT("Colors/GraphBackground")
#define PARAMETER_COLOR_GRAPH_BACKGROUND_DEFAULT                0xFBFBFBu
#define PARAMETER_COLOR_GRAPH_DRAW                              wxT("Colors/GraphDraw")
#define PARAMETER_COLOR_GRAPH_DRAW_DEFAULT                      0xFF4343u
#define PARAMETER_COLOR_GRAPH_TEXT                              wxT("Colors/GraphText")
#define PARAMETER_COLOR_GRAPH_TEXT_DEFAULT                      0x1A1A1Au

#define PARAMETER_DARK_COLOR_WINDOW_BACKGROUND                  wxT("DarkColors/WindowBackground")
#define PARAMETER_DARK_COLOR_WINDOW_BACKGROUND_DEFAULT          0x202020u
#define PARAMETER_DARK_COLOR_CONTROL_TEXT                       wxT("DarkColors/ControlText")
#define PARAMETER_DARK_COLOR_CONTROL_TEXT_DEFAULT               0xFFFFFFu
#define PARAMETER_DARK_COLOR_CONTROL_BACKGROUND                 wxT("DarkColors/ControlBackground")
#define PARAMETER_DARK_COLOR_CONTROL_BACKGROUND_DEFAULT         0x2D2D2Du
#define PARAMETER_DARK_COLOR_GRID_SELECTED_BACKGROUND           wxT("DarkColors/GridSelectedBackground")
#define PARAMETER_DARK_COLOR_GRID_SELECTED_BACKGROUND_DEFAULT   0x0078D4u
#define PARAMETER_DARK_COLOR_GRID_TEXT                          wxT("DarkColors/GridText")
#define PARAMETER_DARK_COLOR_GRID_TEXT_DEFAULT                  0xFFFFFFu
#define PARAMETER_DARK_COLOR_GRID_LINES                         wxT("DarkColors/GridLines")
#define PARAMETER_DARK_COLOR_GRID_LINES_DEFAULT                 0x1D1D1Du
#define PARAMETER_DARK_COLOR_GRID_BACKGROUND                    wxT("DarkColors/GridBackground")
#define PARAMETER_DARK_COLOR_GRID_BACKGROUND_DEFAULT            0x2D2D2Du
#define PARAMETER_DARK_COLOR_GRID_NEW_BACKGROUND                wxT("DarkColors/GridNewBackground")
#define PARAMETER_DARK_COLOR_GRID_NEW_BACKGROUND_DEFAULT        0x00CC6Au
#define PARAMETER_DARK_COLOR_GRID_UPDATED_BACKGROUND            wxT("DarkColors/GridUpdatedBackground")
#define PARAMETER_DARK_COLOR_GRID_UPDATED_BACKGROUND_DEFAULT    0xFF4343u
#define PARAMETER_DARK_COLOR_GRAPH_FRAME                        wxT("DarkColors/GraphFrame")
#define PARAMETER_DARK_COLOR_GRAPH_FRAME_DEFAULT                0x1D1D1Du
#define PARAMETER_DARK_COLOR_GRAPH_BACKGROUND                   wxT("DarkColors/GraphBackground")
#define PARAMETER_DARK_COLOR_GRAPH_BACKGROUND_DEFAULT           0x2D2D2Du
#define PARAMETER_DARK_COLOR_GRAPH_DRAW                         wxT("DarkColors/GraphDraw")
#define PARAMETER_DARK_COLOR_GRAPH_DRAW_DEFAULT                 0xFF4343u
#define PARAMETER_DARK_COLOR_GRAPH_TEXT                         wxT("DarkColors/GraphText")
#define PARAMETER_DARK_COLOR_GRAPH_TEXT_DEFAULT                 0xFFFFFFu

// ѕараметры CAN-шины
struct ParametersCAN
{
public:
    uint32_t Signature;
    uint32_t ServiceID;
    uint8_t  MinimalDataSize;
};

// ѕараметры последовательного порта
struct ParametersSerial
{
public:
    uint32_t PortSpeed;
};

// ѕараметры сети
struct ParametersNetwork
{
public:
    uint16_t UdpPort;
};

// ÷ветовые стили дл€ окна
struct ParametersColors
{
public:
    wxColour WindowBackground;          // цвет окна
    wxColour ControlText;               // цвет текста элементов окна
    wxColour ControlBackground;         // цвет фона элементов окна
    wxColour GridText;                  // цвет текста
    wxColour GridLines;                 // цвет линий
    wxColour GridBackground;            // цвет фона €чеек по умолчанию
    wxColour GridNewBackground;         // цвет фона новых данных
    wxColour GridUpdatedBackground;     // цвет фона обновл€емых данных
    wxColour GridSelectedBackground;    // цвет фона выделенных €чеек
    wxColour GraphFrame;                // цвет рамки графика
    wxColour GraphBackground;           // кисть фоновой заливки графика
    wxColour GraphDraw;                 // цвет линии графика
    wxColour GraphText;                 // цвет текста графика
};

//  ласс настроек
class Parameters
{
public:
    static void Init(const wxString& iniFile);

    static inline bool              isDark;
    static inline ParametersCAN     can;
    static inline ParametersSerial  serial;
    static inline ParametersNetwork network;
    static inline ParametersColors  colors;

private:
    static uint32_t ReadNumber(wxFileConfig& fileConfig, wxString name, uint32_t defaultValue, uint32_t maxValue, bool hex = false);
};
