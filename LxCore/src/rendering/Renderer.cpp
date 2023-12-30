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
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
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

    m_4xMsaaQuality = infrastructure.GetMsaaQuality(DXGI_FORMAT_R8G8B8A8_UNORM, 4);

    infrastructure.CreateDescriptorHeap(m_SwapChainBufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, &m_RtvHeap);
    infrastructure.CreateDescriptorHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, true, &m_DsvHeap);

    CD3DX12_CPU_DESCRIPTOR_HANDLE heapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < m_SwapChainBufferCount; ++i)
    {
        LxHrAssert(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffers[i])), "Failed to get swap chain buffer");
        infrastructure.CreateRenderTargetView(m_SwapChainBuffers[i].Get(), nullptr, heapHandle);
        heapHandle.ptr += m_RtvSize;
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE Renderer::GetBackBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrBackBuffer, m_RtvSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Renderer::GetDepthStencilView() const
{
    return m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
}
