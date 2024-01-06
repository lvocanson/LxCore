#include "LxCore/rendering/Renderer.h"
#include "LxCore/rendering/Renderer.h"
#include "LxCore/core/Window.h"

Renderer::Renderer(Window& window)
{
#if defined(_DEBUG) || defined(DEBUG)
    ID3D12Debug* debugController;
    LxHrAssert(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)), "Failed to create debug controller");
    debugController->EnableDebugLayer();
    debugController->Release();
#endif

    CreateFactory();
    CreateDevice(D3D_FEATURE_LEVEL_11_0);
    
    CreateFence(&m_Fence);
    CreateCommandObjects(&m_CommandQueue, &m_CommandAllocator, &m_CommandList);

    DXGI_SWAP_CHAIN_DESC scd
    {
        .BufferDesc
        {
            .Width = 0,
            .Height = 0,
            .RefreshRate
            {
                .Numerator = 60,
                .Denominator = 1
            },
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM, // TODO: Make this a parameter/variable
            .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
            .Scaling = DXGI_MODE_SCALING_UNSPECIFIED
        },
        .SampleDesc
        {
            .Count = 1,
            .Quality = 0
        },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 2,
        .OutputWindow = window.GetHandle(),
        .Windowed = TRUE,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .Flags = 0

    };
    CreateSwapChain(m_CommandQueue.Get(), &scd, &m_SwapChain);

    m_RtvSize = GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_DsvSize = GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_CbvSrvUavSize = GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    m_4xMsaaQuality = GetMsaaQuality(DXGI_FORMAT_R8G8B8A8_UNORM, 4); // TODO: Make this parameters/variables

    CreateDescriptorHeap(m_SwapChainBufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, &m_RtvHeap);
    CreateDescriptorHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false, &m_DsvHeap);

    CD3DX12_CPU_DESCRIPTOR_HANDLE heapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < m_SwapChainBufferCount; ++i)
    {
        LxHrAssert(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffers[i])), "Failed to get swap chain buffer");
        CreateRenderTargetView(m_SwapChainBuffers[i].Get(), nullptr, heapHandle);
        heapHandle.ptr += m_RtvSize;
    }

    D3D12_RESOURCE_DESC depthStencilDesc
    {
        .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        .Alignment = 0,
        .Width = (UINT)window.GetSize().cx,
        .Height = (UINT)window.GetSize().cy,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_D16_UNORM, // TODO: Make this a parameter/variable
        .SampleDesc
        {
            .Count = 4,
            .Quality = m_4xMsaaQuality - 1
        },
        .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
        .Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
    };
    D3D12_CLEAR_VALUE optClear
    {
        .Format = DXGI_FORMAT_D16_UNORM, // TODO: Make this a parameter/variable
        .DepthStencil
        {
            .Depth = 1.f,
            .Stencil = 0
        }
    };
    D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &optClear, &m_DepthStencilBuffer);

    heapHandle = m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
    CreateDepthStencilView(m_DepthStencilBuffer.Get(), nullptr, heapHandle);

    D3D12_VIEWPORT viewport
    {
        .TopLeftX = 0.f,
        .TopLeftY = 0.f,
        .Width = (float)window.GetSize().cx,
        .Height = (float)window.GetSize().cy,
        .MinDepth = 0.f,
        .MaxDepth = 1.f
    };
    m_CommandList->RSSetViewports(1, &viewport);
}

void Renderer::CreateFence(ID3D12Fence** fence) const
{
    LxHrAssert(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence)), "Failed to create fence");
}

void Renderer::CreateCommandObjects(ID3D12CommandQueue** commandQueue, ID3D12CommandAllocator** commandAllocator, ID3D12GraphicsCommandList** commandList) const
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

void Renderer::CreateSwapChain(ID3D12CommandQueue* commandQueue, DXGI_SWAP_CHAIN_DESC* swapChainDesc, IDXGISwapChain** swapChain) const
{
    LxHrAssert(m_Factory->CreateSwapChain(commandQueue, swapChainDesc, swapChain), "Failed to create swap chain");
}

UINT Renderer::GetDescriptorSize(enum D3D12_DESCRIPTOR_HEAP_TYPE type) const
{
    return m_Device->GetDescriptorHandleIncrementSize(type);
}

void Renderer::CreateDescriptorHeap(UINT numDescriptors, enum D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, ID3D12DescriptorHeap** descriptorHeap) const
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

void Renderer::CreateRenderTargetView(ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const
{
    m_Device->CreateRenderTargetView(resource, desc, handle);
}

void Renderer::CreateCommittedResource(D3D12_HEAP_PROPERTIES* heapProperties, enum D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_DESC* resourceDesc, enum D3D12_RESOURCE_STATES initialState, D3D12_CLEAR_VALUE* clearValue, ID3D12Resource** resource) const
{
    LxHrAssert(m_Device->CreateCommittedResource(heapProperties, heapFlags, resourceDesc, initialState, clearValue, IID_PPV_ARGS(resource)), "Failed to create committed resource");
}

void Renderer::CreateDepthStencilView(ID3D12Resource* resource, D3D12_DEPTH_STENCIL_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const
{
    m_Device->CreateDepthStencilView(resource, desc, handle);
}

UINT Renderer::GetAdapters(std::vector<IDXGIAdapter*>& adapters) const
{
    UINT count = 0;
    for (IDXGIAdapter* adapter = nullptr; m_Factory->EnumAdapters(count, &adapter) != DXGI_ERROR_NOT_FOUND; ++count)
    {
        adapters.push_back(adapter);
    }
    return count;
}

UINT Renderer::GetAdapterOutputs(IDXGIAdapter* adapter, std::vector<IDXGIOutput*>& outputs)
{
    UINT count = 0;
    for (IDXGIOutput* output = nullptr; adapter->EnumOutputs(count, &output) != DXGI_ERROR_NOT_FOUND; ++count)
    {
        outputs.push_back(output);
    }
    return count;
}

UINT Renderer::GetOutputDisplayModes(IDXGIOutput* output, enum DXGI_FORMAT format, UINT flags, std::vector<DXGI_MODE_DESC>& modes)
{
    UINT count = 0;
    output->GetDisplayModeList(format, flags, &count, nullptr);
    modes.resize(count);
    output->GetDisplayModeList(format, flags, &count, modes.data());
    return count;
}

inline void Renderer::CreateFactory()
{
    LxHrAssert(CreateDXGIFactory1(IID_PPV_ARGS(&m_Factory)), "Failed to create DXGI factory");
}

inline void Renderer::CreateDevice(enum D3D_FEATURE_LEVEL featureLevel)
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

UINT Renderer::GetMsaaQuality(enum DXGI_FORMAT format, UINT sampleCount) const
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

D3D12_CPU_DESCRIPTOR_HANDLE Renderer::GetBackBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrBackBuffer, m_RtvSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Renderer::GetDepthStencilView() const
{
    return m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
}
