#include <LxCore/Engine.h>

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    if (LxCore::Init(hInstance, lpCmdLine, nCmdShow))
    {
        LxCore::Run();
    }

    return 0;
}
