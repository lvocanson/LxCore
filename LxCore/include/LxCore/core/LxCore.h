#pragma once
#include "Window.h"
#include "LxCore/rendering/Renderer.h"
#include "Timer.h"

class LxCore
{
public:
    static void InitAndRun(HINSTANCE hInstance, int nCmdShow, const wchar_t* windowName);
    static void Pause();
    static void Resume();
    static void Shutdown();

    static int FPS() { return s_Instance->m_FPS; }
    static float MSPerFrame() { return s_Instance->m_MSPerFrame; }

private:
    LxCore(HINSTANCE hInstance, int nCmdShow, const wchar_t* windowName);
    ~LxCore() = default;
    LxCore(const LxCore&) = delete;
    LxCore& operator=(const LxCore&) = delete;

    void MainLoop();
    void UpdateFrameStats();
    void Cleanup();

private:
    static inline LxCore* s_Instance = nullptr;
    bool m_Running = false;
    bool m_Paused = false;

    Window m_MainWnd;
    Renderer m_Renderer;

    Timer m_GameTimer;
    int m_FPS = 0;
    float m_MSPerFrame = 0.0f;
};
