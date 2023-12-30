#pragma once
#include "Window.h"
#include "LxCore/rendering/Infrastructure.h"
#include "LxCore/rendering/Renderer.h"

class LxCore
{
public:
    static void InitAndRun(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
    static LxCore* Get() { return s_Instance; }
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
    bool m_Running = false;
    Window m_MainWnd;
    Infrastructure m_Infrastructure;
    Renderer m_Renderer;
};
