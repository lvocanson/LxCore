#include "LxCore/rendering/Renderer.h"
#include "LxCore/rendering/Infrastructure.h"
#include "LxCore/core/Window.h"

Renderer::Renderer(Infrastructure& infrastructure, Window& window)
{
    infrastructure.CreateFence(&m_Fence);
    infrastructure.CreateCommandObjects(&m_CommandQueue, &m_CommandAllocator, &m_CommandList);

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
    infrastructure.CreateSwapChain(m_CommandQueue.Get(), &scd, &m_SwapChain);

    m_RtvSize = infrastructure.GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_DsvSize = infrastructure.GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_CbvSrvUavSize = infrastructure.GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    m_4xMsaaQuality = infrastructure.GetMsaaQuality(DXGI_FORMAT_R8G8B8A8_UNORM, 4); // TODO: Make this parameters/variables

    infrastructure.CreateDescriptorHeap(m_SwapChainBufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, &m_RtvHeap);
    infrastructure.CreateDescriptorHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false, &m_DsvHeap);

    CD3DX12_CPU_DESCRIPTOR_HANDLE heapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < m_SwapChainBufferCount; ++i)
    {
        LxHrAssert(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffers[i])), "Failed to get swap chain buffer");
        infrastructure.CreateRenderTargetView(m_SwapChainBuffers[i].Get(), nullptr, heapHandle);
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
    infrastructure.CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &optClear, &m_DepthStencilBuffer);

    heapHandle = m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
    infrastructure.CreateDepthStencilView(m_DepthStencilBuffer.Get(), nullptr, heapHandle);

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

D3D12_CPU_DESCRIPTOR_HANDLE Renderer::GetBackBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrBackBuffer, m_RtvSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Renderer::GetDepthStencilView() const
{
    return m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
}
