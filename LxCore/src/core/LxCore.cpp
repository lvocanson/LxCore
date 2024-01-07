#include "LxCore\core\LxCore.h"
#include "LxCore\core\Window.h"
#include <sstream>

void LxCore::InitAndRun(HINSTANCE hInstance, int nCmdShow, const wchar_t* windowName)
{
    LxAssert(s_Instance == nullptr, "LxCore already created.");
    s_Instance = new LxCore(hInstance, nCmdShow, windowName);
    s_Instance->MainLoop();
    s_Instance->Cleanup();
}

void LxCore::Pause()
{
    LxAssert(s_Instance != nullptr, "LxCore not created.");
    s_Instance->m_Paused = true;
    s_Instance->m_GameTimer.Stop();
}

void LxCore::Resume()
{
    LxAssert(s_Instance != nullptr, "LxCore not created.");
    s_Instance->m_Paused = false;
    s_Instance->m_GameTimer.Resume();
}

void LxCore::Shutdown()
{
    if (s_Instance != nullptr)
        s_Instance->m_Running = false;
}

LxCore::LxCore(HINSTANCE hInstance, int nCmdShow, const wchar_t* windowName)
    : m_MainWnd(hInstance, nCmdShow, windowName)
    , m_Renderer(m_MainWnd)
    , m_GameTimer()
{
    m_MainWnd.SetOnClose(Shutdown);
    m_MainWnd.SetOnResize([]() { s_Instance->m_Renderer.OnResize(); });
}

void LxCore::MainLoop()
{
    m_Running = true;
    while (m_Running)
    {
        m_MainWnd.ProcessMessages();
        m_GameTimer.Tick();
        if (m_Paused)
        {
            Sleep(100);
            continue;
        }

        UpdateFrameStats();
        m_Renderer.Render();
    }
}

void LxCore::UpdateFrameStats()
{
    static USHORT frameCount = 0;
    static float timeElapsed = 0.0f;
    frameCount++;
    timeElapsed += m_GameTimer.DeltaTime();
    if (timeElapsed >= 1.0f)
    {
        m_FPS = frameCount;
        m_MSPerFrame = 1000.0f / m_FPS;

        std::wostringstream outs;
        outs.precision(6);
        outs << L"FPS: " << m_FPS << L" | " << L"Frame Time: " << m_MSPerFrame << L" (ms)";
        m_MainWnd.SetTitle(outs.str().c_str());

        frameCount = 0;
        timeElapsed -= 1.0f;
    }
}

void LxCore::Cleanup()
{
    delete s_Instance;
    s_Instance = nullptr;
}
