#pragma once
#include "Window.h"
#include "LxCore/rendering/Infrastructure.h"

class LxCore
{
public:
    static bool Init(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
    static LxCore* Get() { return s_Instance; }
    static void Run() { s_Instance->MainLoop(); }
    static void Error(const wchar_t* message);
    static void Shutdown();

private:
    LxCore(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
    ~LxCore() = default;
    LxCore(const LxCore&) = delete;
    LxCore& operator=(const LxCore&) = delete;

    void MainLoop();
    void Cleanup();

private:
    static inline LxCore* s_Instance = nullptr;
    bool m_Initialized = false;
    bool m_Running = false;
    Window m_MainWnd;
    Infrastructure m_Infrastructure;
};
