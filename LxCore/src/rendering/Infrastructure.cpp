#include "LxCore/rendering/Infrastructure.h"

Infrastructure::Infrastructure(enum D3D_FEATURE_LEVEL featureLevel)
{
#if defined(_DEBUG) || defined(DEBUG)
    ID3D12Debug* debugController;
    LxHrAssert(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)), "Failed to create debug controller");
    debugController->EnableDebugLayer();
    debugController->Release();
#endif

    LxHrAssert(CreateDXGIFactory1(IID_PPV_ARGS(&m_Factory)), "Failed to create DXGI factory");

    // Create a hardware device if possible, otherwise create a WARP device
    HRESULT hr = D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&m_Device));
    if (FAILED(hr))
    {
        IDXGIAdapter* adapter;
        LxHrAssert(m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)), "Failed to create WARP adapter");
        LxHrAssert(D3D12CreateDevice(adapter, featureLevel, IID_PPV_ARGS(&m_Device)), "Failed to create WARP device");
    }

    m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_DsvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_CbvSrvUavDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void Infrastructure::CreateFence(ID3D12Fence** fence) const
{
    LxHrAssert(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence)), "Failed to create fence");
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

UINT Infrastructure::GetOutputDisplayModes(IDXGIOutput* output, enum DXGI_FORMAT format, UINT flags, std::vector<DXGI_MODE_DESC>& modes)
{
    UINT count = 0;
    output->GetDisplayModeList(format, flags, &count, nullptr);
    modes.resize(count);
    output->GetDisplayModeList(format, flags, &count, modes.data());
    return count;
}
