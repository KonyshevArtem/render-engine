#include "engine_framework.h"
#include "imgui_wrapper.h"

#include <unordered_map>
#include <windows.h>
#include <windowsx.h>

namespace WinMain_Local
{
    std::unordered_map<int, int> s_SpecialKeysMap
    {
        {VK_LSHIFT, 0},
        {VK_RSHIFT, 1},
        {VK_LCONTROL, 2},
        {VK_RCONTROL, 3},
        {VK_RETURN, 4},
        {VK_BACK, 5},
        {VK_DELETE, 6},
        {VK_HOME, 7},
        {VK_END, 8},
        {VK_LEFT, 9},
        {VK_RIGHT, 10},
        {VK_UP, 11},
        {VK_DOWN, 12},
        {VK_OEM_3, 13}
    };
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    struct Data
    {
        HWND Window;
        UINT Message;
        WPARAM WParam;
        LPARAM LParam;
    };

    Data data{window, message, wParam, lParam};
    ImGuiWrapper::ProcessMessage(&data);

    auto getMouseKeyIndex = [](UINT msg)
    {
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
            return 0;
        if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
            return 1;
        if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
            return 2;
        return -1;
    };

    auto isMouseButtonDown = [](UINT msg)
    {
        return msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
    };

    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            EngineFramework::ProcessMouseClick(getMouseKeyIndex(message), isMouseButtonDown(message));
            return 0;
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            const bool isPressed = message == WM_KEYDOWN;
            if (WinMain_Local::s_SpecialKeysMap.contains(wParam))
                EngineFramework::ProcessSpecialKey(WinMain_Local::s_SpecialKeysMap[wParam], isPressed);
            else
                EngineFramework::ProcessKeyPress(static_cast<char>(wParam), isPressed);
            return 0;
        }
        case WM_CHAR:
            EngineFramework::ProcessCharInput(static_cast<wchar_t>(wParam));
            return 0;
        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            EngineFramework::ProcessMouseMove(static_cast<float>(x), static_cast<float>(y));
            return 0;
        }
        default:
            return DefWindowProc(window, message, wParam, lParam);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    WNDCLASS windowClass{};
    windowClass.style = CS_OWNDC;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = L"RenderEngine Window Class";
    RegisterClass(&windowClass);

    HWND window = CreateWindowEx(
            0,
            windowClass.lpszClassName,
            L"Render Engine",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
            nullptr,
            nullptr,
            hInstance,
            nullptr
    );

    if (!window)
        return 0;

    int argc;
    LPWSTR* arguments = CommandLineToArgvW(pCmdLine, &argc);
    std::vector<char*> argv(argc);
    for (int i = 0; i < argc; ++i)
    {
        size_t argSize;
        wcstombs_s(&argSize, nullptr, 0, arguments[i], 0);
        argv[i] = new char[argSize];
        wcstombs_s(nullptr, argv[i], argSize, arguments[i], argSize);
    }

    ShowWindow(window, nCmdShow);

    EngineFramework::Initialize(nullptr, window, argv.data(), argc);

    for (int i = 0; i < argc; ++i)
        delete[] argv[i];

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            if (EngineFramework::ShouldCloseWindow())
                PostQuitMessage(0);

            RECT rect;
            if (GetClientRect(window, &rect))
            {
                int width = rect.right - rect.left;
                int height = rect.bottom - rect.top;
                EngineFramework::TickMainLoop(width, height);
            }
        }
    }

    EngineFramework::Shutdown();
    DestroyWindow(window);

    return 0;
}
