#include "CommandLine.h"


CommandLine cmd;

static constexpr auto CMD_WINDOW_NAME = TEXT("CMD");
static LRESULT WINAPI wndProc(HWND, UINT, WPARAM, LPARAM);

constexpr auto WINDOW_MENU_HEIGHT = 40;
constexpr auto CMD_HEIGHT = 20;

CommandLine::CommandLine()
{
    hCmdWind = nullptr;
    hCmdAccel = nullptr;
}

void CommandLine::init(HWND hParentWindow, HINSTANCE hInstance) {
    /*Register Window Class*/
    WNDCLASSEX wcex = {
        .cbSize = sizeof(WNDCLASSEX),
        /* Win 3.x */
            .style = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = wndProc,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = hInstance,
            .hIcon = LoadIcon(nullptr, MAKEINTRESOURCE(IDI_APPLICATION)),
            .hCursor = LoadCursor(nullptr, MAKEINTRESOURCE(IDC_IBEAM)),
            .hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH),
            .lpszMenuName = nullptr,
            .lpszClassName = CMD_WINDOW_NAME,
            /* Win 4.0 */
                .hIconSm = LoadIcon(nullptr, MAKEINTRESOURCE(IDI_APPLICATION)),
    };
    RegisterClassEx(&wcex);
    
    RECT parRect; //Parent Rectangle
    GetClientRect(hParentWindow, &parRect);
    auto parWidth = parRect.right - parRect.left;
    auto parHeight = parRect.bottom - parRect.top;

    constexpr auto winStyle = WS_CHILDWINDOW | WS_OVERLAPPED;
    const auto winExStyle = 0L;
    hCmdWind = CreateWindowEx(winExStyle, CMD_WINDOW_NAME, CMD_WINDOW_NAME, winStyle,
        //Default Position
        0, parHeight - CMD_HEIGHT,
        //Default Size
        parWidth, CMD_HEIGHT,
        hParentWindow, nullptr, hInstance, nullptr);

    ShowWindow(hCmdWind, SW_SHOW);
    UpdateWindow(hCmdWind);

    hCmdAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_CMDLINE));

}


LRESULT WINAPI wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_KEYDOWN:
    {
        if ( ('A' <= wParam && wParam <= 'Z') or 
            wParam == ' ' or 
            ('0' <= wParam && wParam <= '9')) //in range between A and Z
        {
            cmd.commandLine.push_back((char)wParam);

        }
        else if (wParam == VK_BACK)
        {
            if(cmd.commandLine.size() > 0)
                cmd.commandLine.pop_back();
        }


        RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE);
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (LOWORD(wParam)) //for WM_COMMAND the wParam is the command
        {
        case IDM_EXIT:
            DestroyWindow(ghMainWindow);
            break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        break;
    }
    
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        auto hDC = BeginPaint(hWnd, &ps);
        TextOutA(hDC, 0, 0, cmd.commandLine.c_str(), 
            cmd.commandLine.size()+1);

        EndPaint(hWnd, &ps);
        break;
    }




    case WM_LBUTTONDOWN:
    {
        if(GetFocus() != hWnd) SetFocus(hWnd);
        break;
    }
    case WM_SETFOCUS:
    {
        ghActiveAccelTable = cmd.hCmdAccel;
        break;
    }
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}