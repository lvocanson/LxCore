#include "LxCore\core\LxCore.h"
#include "LxCore\core\Window.h"
#include "LxCore/rendering/Infrastructure.h"
#include <sstream>

void LxCore::InitAndRun(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    LxAssert(s_Instance == nullptr, "LxCore already created.");
    s_Instance = new LxCore(hInstance, lpCmdLine, nCmdShow);
    s_Instance->MainLoop();
    s_Instance->Cleanup();
}

void LxCore::Shutdown()
{
    if (s_Instance != nullptr)
        s_Instance->m_Running = false;
}

LxCore::LxCore(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
    : m_MainWnd(hInstance, nCmdShow, Shutdown)
    , m_Infrastructure(D3D_FEATURE_LEVEL_11_0)
    , m_Renderer(m_Infrastructure, m_MainWnd)
    , m_GameTimer()
{
}

void LxCore::MainLoop()
{
    m_Running = true;
    while (m_Running)
    {
        m_MainWnd.ProcessMessages();
        m_GameTimer.Tick();
        UpdateFrameStats();
    }
}

void LxCore::UpdateFrameStats()
{
    static int frameCount = 0;
    static float timeElapsed = 0.0f;
    frameCount++;
    timeElapsed += m_GameTimer.DeltaTime();
    if (timeElapsed >= 1.0f)
    {
        m_FPS = (float)frameCount;
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
