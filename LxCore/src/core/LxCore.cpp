#include "LxCore\core\LxCore.h"
#include "LxCore\core\Window.h"
#include "LxCore/rendering/Infrastructure.h"
#include <sstream>

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

void LxCore::Error(const wchar_t* message)
{
    MessageBoxW(NULL, message, L"Error", MB_OK | MB_ICONERROR);
}

void LxCore::Shutdown()
{
    if (s_Instance != nullptr)
        s_Instance->m_Running = false;
}

LxCore::LxCore(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
    : m_MainWnd(hInstance, nCmdShow, Shutdown), m_Infrastructure()
{
    if (!m_MainWnd.IsCreated())
    {
        Error(m_MainWnd.GetLastErrorMessage());
        return;
    }

    if (!m_Infrastructure.Init())
    {
        Error(m_Infrastructure.GetLastErrorMessage());
        return;
    }

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
