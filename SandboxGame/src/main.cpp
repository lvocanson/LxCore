#include <LxCore/Engine.h>

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
    LxCore::InitAndRun(hInstance, nCmdShow, L"Game");
    return 0;
}
