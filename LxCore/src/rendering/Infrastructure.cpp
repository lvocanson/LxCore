#include "LxCore/rendering/Infrastructure.h"

bool Infrastructure::Init()
{
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_Factory));
    if (FAILED(hr))
    {
        m_LastErrorMessage = L"Failed to create DXGI factory";
        return false;
    }

    return true;
}

UINT Infrastructure::GetAdapters(std::vector<IDXGIAdapter*>& adapters) const
{
    UINT count = 0;
    for (IDXGIAdapter* adapter = nullptr; m_Factory->EnumAdapters(count, &adapter) != DXGI_ERROR_NOT_FOUND; ++count)
    {
        adapters.push_back(adapter);
    }
    return count;
}

UINT Infrastructure::GetAdapterOutputs(IDXGIAdapter* adapter, std::vector<IDXGIOutput*>& outputs)
{
    UINT count = 0;
    for (IDXGIOutput* output = nullptr; adapter->EnumOutputs(count, &output) != DXGI_ERROR_NOT_FOUND; ++count)
    {
        outputs.push_back(output);
    }
    return count;
}

UINT Infrastructure::GetOutputDisplayModes(IDXGIOutput* output, enum DXGI_FORMAT format, std::vector<DXGI_MODE_DESC>& modes)
{
    UINT count = 0;
    UINT flags = 0;

    output->GetDisplayModeList(format, flags, &count, nullptr);
    modes.resize(count);
    output->GetDisplayModeList(format, flags, &count, modes.data());
    return count;
}
