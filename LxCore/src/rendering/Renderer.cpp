#include "LxCore/rendering/Renderer.h"
#include "LxCore/core/Window.h"
#include <DirectXColors.h>

Renderer::Renderer(Window& window)
    : m_Window(window)
{
#if defined(_DEBUG) || defined(DEBUG)
    ID3D12Debug* debugController;
    LxHrAssert(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)), "Failed to create debug controller");
    debugController->EnableDebugLayer();
    debugController->Release();
#endif

    LxHrAssert(CreateDXGIFactory1(IID_PPV_ARGS(&m_Factory)), "Failed to create DXGI factory");

    // Create a hardware device if possible, otherwise create a WARP device
    HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)); // TODO: Make feature level a parameter/variable
    if (FAILED(hr))
    {
        IDXGIAdapter* adapter;
        LxHrAssert(m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)), "Failed to create WARP adapter");
        LxHrAssert(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)), "Failed to create WARP device");
    }
    LxHrAssert(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)), "Failed to create fence");

    CreateCommandObjects();
    CreateSwapChain();

    m_RtvSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_DsvSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_CbvSrvUavSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    CreateRtvAndDsvDescriptorHeap();

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels
    {
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM, // TODO: Make this a parameter/variable
        .SampleCount = 4,
        .Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE
    };
    LxHrAssert(m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)), "Failed to check multisample quality levels");
    LxAssert(msQualityLevels.NumQualityLevels > 0, "Unexpected 4x MSAA quality level");
    m_4xMsaaQuality = msQualityLevels.NumQualityLevels;

    OnResize();
}

void Renderer::OnResize()
{
    LxAssert(m_Device != nullptr, "Device not created");

    // Wait until the GPU is done processing commands
    FlushCommandQueue();

    // Reset for recreation
    LxHrAssert(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr), "Failed to reset command list");
    for (UINT i = 0; i < m_SwapChainBufferCount; ++i)
    {
        m_SwapChainBuffers[i].Reset();
    }
    m_DepthStencilBuffer.Reset();

    LxHrAssert(m_SwapChain->ResizeBuffers(
        m_SwapChainBufferCount,
        (UINT)m_Window.GetSize().cx,
        (UINT)m_Window.GetSize().cy,
        DXGI_FORMAT_R8G8B8A8_UNORM, // TODO: Make this a parameter/variable
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH), "Failed to resize swap chain buffers");

    m_CurrBackBuffer = 0;
    CD3DX12_CPU_DESCRIPTOR_HANDLE heapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < m_SwapChainBufferCount; ++i)
    {
        LxHrAssert(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffers[i])), "Failed to get swap chain buffer");
        m_Device->CreateRenderTargetView(m_SwapChainBuffers[i].Get(), nullptr, heapHandle);
        heapHandle.ptr += m_RtvSize;
    }
    D3D12_RESOURCE_DESC depthStencilDesc
    {
        .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        .Alignment = 0,
        .Width = (UINT)m_Window.GetSize().cx,
        .Height = (UINT)m_Window.GetSize().cy,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_D16_UNORM, // TODO: Make this a parameter/variable
        .SampleDesc
        {
            .Count = m_4xMsaaEnabled ? 4u : 1u,
            .Quality = m_4xMsaaEnabled ? (m_4xMsaaQuality - 1u) : 0u
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
    LxHrAssert(m_Device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &optClear, IID_PPV_ARGS(&m_DepthStencilBuffer)), "Failed to create depth stencil buffer");

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(m_DsvHeap->GetCPUDescriptorHandleForHeapStart());
    m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), nullptr, dsvHeapHandle);

    LxHrAssert(m_CommandList->Close(), "Failed to close command list");
    ID3D12CommandList* commandLists[] = {m_CommandList.Get()};
    m_CommandQueue->ExecuteCommandLists(1, commandLists);

    m_ScreenViewport = {
        .TopLeftX = 0.f,
        .TopLeftY = 0.f,
        .Width = (float)m_Window.GetSize().cx,
        .Height = (float)m_Window.GetSize().cy,
        .MinDepth = 0.f,
        .MaxDepth = 1.f
    };

    m_ScissorRect = {
        .left = 0,
        .top = 0,
        .right = (LONG)m_Window.GetSize().cx,
        .bottom = (LONG)m_Window.GetSize().cy
    };

    // Wait until the GPU is done with the resize
    FlushCommandQueue();
}

void Renderer::Render()
{
    LxHrAssert(m_CommandAllocator->Reset(), "Failed to reset command allocator");
    LxHrAssert(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr), "Failed to reset command list");
    D3D12_RESOURCE_BARRIER toRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
        m_SwapChainBuffers[m_CurrBackBuffer].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_CommandList->ResourceBarrier(1, &toRenderTarget);

    m_CommandList->RSSetViewports(1, &m_ScreenViewport);
    m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CurrentBackBufferView();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DepthStencilView();

    m_CommandList->ClearRenderTargetView(rtvHandle, DirectX::Colors::Magenta, 0, nullptr);
    m_CommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);

    m_CommandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

    D3D12_RESOURCE_BARRIER toPresent = CD3DX12_RESOURCE_BARRIER::Transition(
        m_SwapChainBuffers[m_CurrBackBuffer].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
    m_CommandList->ResourceBarrier(1, &toPresent);

    LxHrAssert(m_CommandList->Close(), "Failed to close command list");
    ID3D12CommandList* commandLists[] = {m_CommandList.Get()};
    m_CommandQueue->ExecuteCommandLists(1, commandLists);

    LxHrAssert(m_SwapChain->Present(0, 0), "Failed to present swap chain");
    m_CurrBackBuffer = (m_CurrBackBuffer + 1) % m_SwapChainBufferCount;

    FlushCommandQueue(); // TODO: Temp, remove later
}

UINT Renderer::GetAdapters(std::vector<IDXGIAdapter*>& adapters) const
{
    LxAssert(m_Factory != nullptr, "Factory not created");

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

void Renderer::CreateCommandObjects()
{
    LxAssert(m_Device != nullptr, "Device not created");

    D3D12_COMMAND_QUEUE_DESC queueDesc
    {
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0
    };
    LxHrAssert(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)), "Failed to create command queue");
    LxHrAssert(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)), "Failed to create command allocator");
    LxHrAssert(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList)), "Failed to create command list");
    m_CommandList->Close();
}

void Renderer::CreateSwapChain()
{
    LxAssert(m_Factory != nullptr, "Factory not created");
    LxAssert(m_CommandQueue != nullptr, "Command queue not created");

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
            .Count = m_4xMsaaEnabled ? 4u : 1u,
            .Quality = m_4xMsaaEnabled ? (m_4xMsaaQuality - 1u) : 0u
        },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 2,
        .OutputWindow = m_Window.GetHandle(),
        .Windowed = TRUE,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .Flags = 0

    };
    LxHrAssert(m_Factory->CreateSwapChain(m_CommandQueue.Get(), &scd, &m_SwapChain), "Failed to create swap chain");
}

void Renderer::CreateRtvAndDsvDescriptorHeap()
{
    LxAssert(m_Device != nullptr, "Device not created");

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc
    {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        .NumDescriptors = m_SwapChainBufferCount,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0
    };
    LxHrAssert(m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RtvHeap)), "Failed to create RTV descriptor heap");

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc
    {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
        .NumDescriptors = 1,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0
    };
    LxHrAssert(m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DsvHeap)), "Failed to create DSV descriptor heap");
}

void Renderer::FlushCommandQueue()
{
    LxAssert(m_Fence != nullptr, "Fence not created");
    LxAssert(m_CommandQueue != nullptr, "Command queue not created");

    LxHrAssert(m_CommandQueue->Signal(m_Fence.Get(), ++m_CurrFence), "Failed to signal fence");

    if (m_Fence->GetCompletedValue() < m_CurrFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
        LxAssert(eventHandle != nullptr, "Failed to create event handle");

        LxHrAssert(m_Fence->SetEventOnCompletion(m_CurrFence, eventHandle), "Failed to set event on completion");
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

inline D3D12_CPU_DESCRIPTOR_HANDLE Renderer::CurrentBackBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_RtvHeap->GetCPUDescriptorHandleForHeapStart(),
        m_CurrBackBuffer, m_RtvSize);
}

inline D3D12_CPU_DESCRIPTOR_HANDLE Renderer::DepthStencilView() const
{
    return m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
}
