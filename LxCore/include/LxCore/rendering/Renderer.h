#pragma once
#include <wrl.h>

class Infrastructure;
class Window;

struct D32D12_CPU_DESCRIPTOR_HANDLE;

struct ID3D12Fence;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct IDXGISwapChain;
struct ID3D12DescriptorHeap;

class Renderer
{
public:
    Renderer(Infrastructure& infrastructure, Window& window);
    ~Renderer() = default;

    D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferView() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

private:
    Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;

    UINT m_RtvSize, m_DsvSize, m_CbvSrvUavSize;
    UINT m_4xMsaaQuality;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;
    static const char m_SwapChainBufferCount = 2;
    char m_CurrBackBuffer = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffers[m_SwapChainBufferCount];
    Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;
};
