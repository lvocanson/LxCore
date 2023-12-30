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
}
