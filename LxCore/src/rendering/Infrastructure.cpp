#include "LxCore/rendering/Infrastructure.h"

Infrastructure::Infrastructure(enum D3D_FEATURE_LEVEL featureLevel)
{
#if defined(_DEBUG) || defined(DEBUG)
    ID3D12Debug* debugController;
    LxHrAssert(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)), "Failed to create debug controller");
    debugController->EnableDebugLayer();
    debugController->Release();
#endif

    CreateFactory();
    CreateDevice(featureLevel);
}

void Infrastructure::CreateFence(ID3D12Fence** fence) const
{
    LxHrAssert(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence)), "Failed to create fence");
}

void Infrastructure::CreateCommandObjects(ID3D12CommandQueue** commandQueue, ID3D12CommandAllocator** commandAllocator, ID3D12GraphicsCommandList** commandList) const
{
    D3D12_COMMAND_QUEUE_DESC queueDesc
    {
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0
    };
    LxHrAssert(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue)), "Failed to create command queue");
    LxHrAssert(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator)), "Failed to create command allocator");
    LxHrAssert(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, *commandAllocator, nullptr, IID_PPV_ARGS(commandList)), "Failed to create command list");
    (*commandList)->Close();
}

void Infrastructure::CreateSwapChain(ID3D12CommandQueue* commandQueue, DXGI_SWAP_CHAIN_DESC* swapChainDesc, IDXGISwapChain** swapChain) const
{
    LxHrAssert(m_Factory->CreateSwapChain(commandQueue, swapChainDesc, swapChain), "Failed to create swap chain");
}

UINT Infrastructure::GetDescriptorSize(enum D3D12_DESCRIPTOR_HEAP_TYPE type) const
{
    return m_Device->GetDescriptorHandleIncrementSize(type);
}

void Infrastructure::CreateDescriptorHeap(UINT numDescriptors, enum D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, ID3D12DescriptorHeap** descriptorHeap) const
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc
    {
        .Type = type,
        .NumDescriptors = numDescriptors,
        .Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0
    };
    LxHrAssert(m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(descriptorHeap)), "Failed to create descriptor heap");
}

void Infrastructure::CreateRenderTargetView(ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const
{
    m_Device->CreateRenderTargetView(resource, desc, handle);
}

void Infrastructure::CreateCommittedResource(D3D12_HEAP_PROPERTIES* heapProperties, enum D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_DESC* resourceDesc, enum D3D12_RESOURCE_STATES initialState, D3D12_CLEAR_VALUE* clearValue, ID3D12Resource** resource) const
{
LxHrAssert(m_Device->CreateCommittedResource(heapProperties, heapFlags, resourceDesc, initialState, clearValue, IID_PPV_ARGS(resource)), "Failed to create committed resource");
}

void Infrastructure::CreateDepthStencilView(ID3D12Resource* resource, D3D12_DEPTH_STENCIL_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const
{
    m_Device->CreateDepthStencilView(resource, desc, handle);
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

inline void Infrastructure::CreateFactory()
{
    LxHrAssert(CreateDXGIFactory1(IID_PPV_ARGS(&m_Factory)), "Failed to create DXGI factory");
}

inline void Infrastructure::CreateDevice(enum D3D_FEATURE_LEVEL featureLevel)
{
    // Create a hardware device if possible, otherwise create a WARP device
    HRESULT hr = D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&m_Device));
    if (FAILED(hr))
    {
        IDXGIAdapter* adapter;
        LxHrAssert(m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)), "Failed to create WARP adapter");
        LxHrAssert(D3D12CreateDevice(adapter, featureLevel, IID_PPV_ARGS(&m_Device)), "Failed to create WARP device");
    }
}

UINT Infrastructure::GetMsaaQuality(enum DXGI_FORMAT format, UINT sampleCount) const
{
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels
    {
        .Format = format,
        .SampleCount = sampleCount,
        .Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE
    };
    LxHrAssert(m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)), "Failed to check multisample quality levels");
    LxAssert(msQualityLevels.NumQualityLevels > 0, "Unexpected 4x MSAA quality level");
    return msQualityLevels.NumQualityLevels;
}

