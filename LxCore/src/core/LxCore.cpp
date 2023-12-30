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
{
}

void LxCore::MainLoop()
{
    m_Running = true;
    while (m_Running)
    {
        m_MainWnd.ProcessMessages();
    }
}

void LxCore::Cleanup()
{
    delete s_Instance;
    s_Instance = nullptr;
}
