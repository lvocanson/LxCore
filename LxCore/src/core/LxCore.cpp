#include "LxCore\core\LxCore.h"
#include "LxCore\core\Window.h"

bool LxCore::Init(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    if (s_Instance != nullptr)
        return false;

    s_Instance = new LxCore(hInstance, lpCmdLine, nCmdShow);

    if (s_Instance->m_Initialized)
        return true;

    delete s_Instance;
    s_Instance = nullptr;
    return false;
}

void LxCore::Shutdown()
{
    s_Instance->m_Running = false;
}

LxCore::LxCore(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
    : m_MainWnd(hInstance, nCmdShow, Shutdown)
{
    if (m_MainWnd.GetHandle() == nullptr)
        return;

    m_Initialized = true;
}

void LxCore::MainLoop()
{
    if (!m_Initialized || m_Running)
        return;

    m_Running = true;
    while (m_Running)
    {
        m_MainWnd.ProcessMessages();
    }

    Cleanup();
}

void LxCore::Cleanup()
{
    m_Running = false;
    delete s_Instance;
    s_Instance = nullptr;
}
