#include "LxCore\core\Window.h"
#include "LxCore\core\LxCore.h"
#include <windowsx.h>

Window::Window(HINSTANCE hInstance, int nCmdShow)
{
    LxAssert(Register(hInstance), "Failed to register window class.");

    m_Handle = CreateWindowExW(
        0,
        s_ClassName, s_WindowName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    LxAssert(m_Handle, "Failed to create window.");

    // Set the user data to this pointer
    SetWindowLongPtrW(m_Handle, GWLP_USERDATA, (LONG_PTR)this);
    ShowWindow(m_Handle, nCmdShow);
    UpdateWindow(m_Handle);

    // Get the window size
    RECT rect;
    GetClientRect(m_Handle, &rect);
    m_Size = {rect.right - rect.left, rect.bottom - rect.top};
}

Window::~Window()
{
    if (m_Handle)
    {
        DestroyWindow(m_Handle);
    }
}

void Window::ProcessMessages()
{
    while (PeekMessageW(&m_Msg, m_Handle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&m_Msg);
        DispatchMessageW(&m_Msg);
    }
}

bool Window::Register(HINSTANCE hInstance)
{
    // Check if the class is already registered
    WNDCLASSEXW wcex;
    if (GetClassInfoExW(hInstance, s_ClassName, &wcex))
    {
        return true;
    }

    // Register the class
    wcex =
    {
        .cbSize = sizeof(WNDCLASSEXW),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = WndProc,
        .cbClsExtra = 0,
        .cbWndExtra = sizeof(Window*),
        .hInstance = hInstance,
        .hIcon = 0,
        .hCursor = LoadCursorW(NULL, IDC_ARROW),
        .hbrBackground = GetSysColorBrush(COLOR_WINDOW),
        .lpszMenuName = NULL,
        .lpszClassName = s_ClassName,
    };
    if (RegisterClassExW(&wcex))
    {
        return true;
    }

    return false;
}

// Get the Window* from the user data
inline Window* GetWindow(HWND hWnd)
{
    return reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
    {
        Window* window = GetWindow(hWnd);
        window->m_OnClose();
        PostQuitMessage(0);
        break;
    }
    case WM_SIZE:
    {
        Window* window = GetWindow(hWnd);
        window->m_Size = {LOWORD(lParam), HIWORD(lParam)};
        if (wParam == SIZE_MAXIMIZED || (wParam == SIZE_RESTORED && !window->m_Resizing))
        {
            window->m_OnResize();
        }
        break;
    }
    case WM_ENTERSIZEMOVE:
    {
        Window* window = GetWindow(hWnd);
        window->m_Resizing = true;
        LxCore::Pause();
        break;
    }
    case WM_EXITSIZEMOVE:
    {
        Window* window = GetWindow(hWnd);
        window->m_Resizing = false;
        window->m_OnResize();
        LxCore::Resume();
        break;
    }
    // Avoid beeping when pressing Alt+Enter
    case WM_MENUCHAR: return MAKELRESULT(0, MNC_CLOSE);

    case WM_KEYDOWN:
    {
        Window* window = GetWindow(hWnd);
        window->m_OnMouseClick(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    }
    case WM_KEYUP:
    {
        Window* window = GetWindow(hWnd);
        window->m_OnKeyInput(wParam, false);
        break;
    }
    case WM_MOUSEMOVE:
    {
        Window* window = GetWindow(hWnd);
        window->m_OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    }
    case WM_MOUSEWHEEL:
    {
        Window* window = GetWindow(hWnd);
        window->m_OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        break;
    }

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
