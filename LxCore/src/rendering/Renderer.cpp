#include "LxCore/rendering/Renderer.h"
#include "LxCore/rendering/Infrastructure.h"

Renderer::Renderer(Infrastructure& infrastructure)
    : m_infrastructure(infrastructure)
{
    infrastructure.CreateFence(&m_fence);
}
