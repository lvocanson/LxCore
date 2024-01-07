#pragma once
#include <vector>
#include <wrl.h>
#include <d3d12.h>

class Window;

struct IDXGIAdapter;
struct IDXGIOutput;
struct DXGI_MODE_DESC;
struct IDXGIFactory4;
struct IDXGISwapChain;

class Renderer
{
public:
    Renderer(Window& window);
    ~Renderer() = default;

    void OnResize();
    void Render();

    UINT GetAdapters(std::vector<IDXGIAdapter*>& adapters) const;
    static UINT GetAdapterOutputs(IDXGIAdapter* adapter, std::vector<IDXGIOutput*>& outputs);
    static UINT GetOutputDisplayModes(IDXGIOutput* output, enum DXGI_FORMAT format, UINT flags, std::vector<DXGI_MODE_DESC>& modes);

private:
    void CreateCommandObjects();
    void CreateSwapChain();
    void CreateRtvAndDsvDescriptorHeap();

    void FlushCommandQueue();

    inline D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
    inline D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

private:
    Window& m_Window;

    Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
    Microsoft::WRL::ComPtr<ID3D12Device> m_Device;

    Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
    UINT m_CurrFence = 0;

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;

    UINT m_RtvSize, m_DsvSize, m_CbvSrvUavSize;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;

    UINT m_4xMsaaQuality;
    bool m_4xMsaaEnabled = false;

    static constexpr BYTE m_SwapChainBufferCount = 2;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffers[m_SwapChainBufferCount];
    BYTE m_CurrBackBuffer = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;

    D3D12_VIEWPORT m_ScreenViewport;
    D3D12_RECT m_ScissorRect;
};
