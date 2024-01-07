#pragma once
#include "LxCore/WindowsCheck.h"
#include <Windows.h>

class Window
{
    typedef void(*Callback)();
    typedef void(*KeyCallback)(unsigned long long key, bool pressed);
    typedef void(*ClickCallback)(unsigned long long key, int x, int y);
    typedef void(*MoveCallback)(int x, int y);
    typedef void(*WheelCallback)(int delta);

public:
    Window(HINSTANCE hInstance, int nCmdShow, const wchar_t* windowName);
    ~Window();

    void ProcessMessages();
    HWND GetHandle() const { return m_Handle; }
    void SetOnClose(Callback callback) { m_OnClose = callback ? callback : DefaultCallback; }

    void SetTitle(const wchar_t* title) { SetWindowTextW(m_Handle, title); }
    bool IsVisible() const { return IsWindowVisible(m_Handle); }

    SIZE GetSize() const { return m_Size; }
    float GetAspectRatio() const { return static_cast<float>(m_Size.cx) / m_Size.cy; }
    bool IsResizing() const { return m_Resizing; }
    void SetOnResize(Callback callback) { m_OnResize = callback ? callback : DefaultCallback; }

    void SetOnKeyInput(KeyCallback callback) { m_OnKeyInput = callback ? callback : DefaultKeyCallback; }
    void SetOnMouseClick(ClickCallback callback) { m_OnMouseClick = callback ? callback : DefaultClickCallback; }
    void SetOnMouseMove(MoveCallback callback) { m_OnMouseMove = callback ? callback : DefaultMoveCallback; }
    void SetOnMouseWheel(WheelCallback callback) { m_OnMouseWheel = callback ? callback : DefaultWheelCallback; }

private:
    static bool Register(HINSTANCE hInstance);
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    HWND m_Handle = nullptr;
    MSG m_Msg = {0};
    Callback m_OnClose = DefaultCallback;

    SIZE m_Size = {0};
    bool m_Resizing = false;
    Callback m_OnResize = DefaultCallback;

    KeyCallback m_OnKeyInput = DefaultKeyCallback;
    ClickCallback m_OnMouseClick = DefaultClickCallback;
    MoveCallback m_OnMouseMove = DefaultMoveCallback;
    WheelCallback m_OnMouseWheel = DefaultWheelCallback;

private: // Default empty callbacks, to avoid null checks
    static void DefaultCallback() {}
    static void DefaultKeyCallback(unsigned long long, bool) {}
    static void DefaultClickCallback(unsigned long long, int, int) {}
    static void DefaultMoveCallback(int, int) {}
    static void DefaultWheelCallback(int) {}

private:
    static constexpr const wchar_t* s_ClassName = L"LxCoreWindow";
};
