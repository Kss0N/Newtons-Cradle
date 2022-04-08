#include "timeCounter.h"
#include "CradleCradle.h"
#include <sstream>


timeCounter tc;


static constexpr auto WINDOW_NAME = TEXT("Counter");


void timeCounter::updateTime(long ms) 
{
    milliseconds = ms;
    InvalidateRect(hWnd, NULL, TRUE);
}

static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_PAINT)
    {
        PAINTSTRUCT ps;
        auto hDC = BeginPaint(hWnd, &ps);
        std::wstringstream wss;
        wss << tc.milliseconds << "ms";
        TextOut(hDC, 0, 0, wss.str().c_str(), wss.str().size());

        EndPaint(hWnd, &ps);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void timeCounter::init()
{
	WNDCLASSEX wcex = {
        .cbSize = sizeof(WNDCLASSEX),
        /* Win 3.x */
        .style = CS_VREDRAW | CS_HREDRAW,
        .lpfnWndProc = WndProc,
        .cbClsExtra = 0,
        .cbWndExtra  = 0,
        .hInstance = ghInstance,
        .hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION)),
        .hCursor = LoadIcon(NULL, MAKEINTRESOURCE(IDC_ARROW)),
        .hbrBackground = (HBRUSH)COLOR_WINDOW+1,
        .lpszMenuName = NULL,
        .lpszClassName = WINDOW_NAME,
        /* Win 4.0 */
        .hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION))
	};
    RegisterClassEx(&wcex);

    auto style = WS_CHILD;
    hWnd = CreateWindow(WINDOW_NAME, WINDOW_NAME, style, 
        0, 0, 
        200, 200, 
        ghMainWindow, NULL, ghInstance, NULL);
    milliseconds = 0;
    ShowWindow(hWnd, SW_SHOW);
}
