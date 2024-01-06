#pragma once
#include <vector>
#include <wrl.h>

class Window;

struct IDXGIFactory4;
struct ID3D12Device;
struct ID3D12Fence;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct IDXGISwapChain;
struct ID3D12DescriptorHeap;
struct ID3D12Resource;

struct IDXGIAdapter;
struct IDXGIOutput;
struct DXGI_MODE_DESC;

class Renderer
{
public:
    Renderer(Window& window);
    ~Renderer() = default;

    void OnResize();

private:
    void CreateFactory();
    void CreateDevice(enum D3D_FEATURE_LEVEL featureLevel);

    void CreateFence();
    void CreateCommandObjects();
    void CreateSwapChain();

    void Get4xMsaaQuality(enum DXGI_FORMAT format);
    void CreateDescriptorHeap(UINT numDescriptors, enum D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, ID3D12DescriptorHeap** descriptorHeap) const;

public:
    UINT GetAdapters(std::vector<IDXGIAdapter*>& adapters) const;
    static UINT GetAdapterOutputs(IDXGIAdapter* adapter, std::vector<IDXGIOutput*>& outputs);
    static UINT GetOutputDisplayModes(IDXGIOutput* output, enum DXGI_FORMAT format, UINT flags, std::vector<DXGI_MODE_DESC>& modes);

private:
    void FlushCommandQueue();

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
    UINT m_4xMsaaQuality;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;
    static const char m_SwapChainBufferCount = 2;
    char m_CurrBackBuffer = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffers[m_SwapChainBufferCount];
    Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;
};
