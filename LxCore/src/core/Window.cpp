#include "LxCore\core\Window.h"

Window::Window(HINSTANCE hInstance, int nCmdShow, Callback onClose)
    : m_OnClose(onClose)
{
    if (!Register(hInstance))
    {
        m_LastErrorMessage = L"Failed to register window class.";
        return;
    }

    m_Handle = CreateWindowExW(
        0,
        s_ClassName, s_WindowName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    if (!m_Handle)
    {
        m_LastErrorMessage = L"Failed to create window.";
        return;
    }

    // Set the user data to this pointer
    SetWindowLongPtrW(m_Handle, GWLP_USERDATA, (LONG_PTR)this);
    ShowWindow(m_Handle, nCmdShow);
    UpdateWindow(m_Handle);

    m_Created = true;
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
        if (window->m_OnClose)
            window->m_OnClose();
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
