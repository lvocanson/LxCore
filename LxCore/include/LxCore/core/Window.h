#pragma once
#include "LxCore/WindowsCheck.h"
#include <Windows.h>

class Window
{
    typedef void(*Callback)();

public:
    Window(HINSTANCE hInstance, int nCmdShow, Callback onClose = nullptr);
    ~Window();

    void ProcessMessages();
    HWND GetHandle() const { return m_Handle; }
    void SetOnClose(Callback callback) { m_OnClose = callback; }

    LONG GetWidth() const { return m_Size.cx; }
    LONG GetHeight() const { return m_Size.cy; }

private:
    static bool Register(HINSTANCE hInstance);
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    HWND m_Handle = nullptr;
    Callback m_OnClose = nullptr;
    MSG m_Msg = {0};
    SIZE m_Size = {0};

private:
    static constexpr const wchar_t* s_ClassName = L"LxCoreWindow";
    static constexpr const wchar_t* s_WindowName = L"LxCoreWindow";
};
