#include "LxCore/rendering/Renderer.h"
#include "LxCore/rendering/Infrastructure.h"

Renderer::Renderer(Infrastructure& infrastructure)
    : m_Infrastructure(infrastructure)
{
    infrastructure.CreateFence(&m_Fence);
    infrastructure.CreateCommandObjects(&m_CommandQueue, &m_CommandAllocator, &m_CommandList);
}
