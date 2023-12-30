#pragma once
#include <wrl.h>

class Infrastructure;
class Window;

struct ID3D12Fence;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct IDXGISwapChain;

class Renderer
{
public:
    Renderer(Infrastructure& infrastructure, Window& window);
    ~Renderer() = default;

private:
    Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
};
