#pragma once
#include <wrl.h>

class Infrastructure;

struct ID3D12Fence;

class Renderer
{
public:
    Renderer(Infrastructure& infrastructure);
    ~Renderer() = default;

private:
    Infrastructure& m_infrastructure;
    Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
};
