#pragma once
#include <wrl.h>

class Infrastructure;

struct ID3D12Fence;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;

class Renderer
{
public:
    Renderer(Infrastructure& infrastructure);
    ~Renderer() = default;

private:
    Infrastructure& m_Infrastructure;
    Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
};
