//Sebastian Lorensson 2022-11-27
#include "pch.h"
#include "dxh.h"

HWND SetupWindow(int height, int width, int x, int y, HINSTANCE hInstance);
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
    static HWND handle = SetupWindow(800, 600, 560, 200, hInstance);

    if (!handle)
    {
        util::ErrorMessageBox("Call to setup window failed. Last err: " + LASTERR);
        return EXIT_FAILURE;
    }

    DXHandler dxh(handle);

    util::DeltaTimer dt;

    MSG msg{};

    ShowWindow(handle, nCmdShow);

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else 
        {
            dxh.Render(dt.Delta());
        }
    }

    return static_cast<int>(msg.wParam);
}

HWND SetupWindow(int width, int height, int x, int y, HINSTANCE hInstance)
{
    TCHAR szWindowClass[] = _T("Hello Triangle");
    TCHAR szTitle[] = _T("Hello Triangle");

    WNDCLASSEX wcex{ 0 };

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        util::ErrorMessageBox("Could not register WNDCLASSEX.");
        exit(-1);
    }

    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        x, y, //center, a bit up
        width, height,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    return hWnd;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
