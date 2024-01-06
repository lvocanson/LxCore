#pragma once
#include <vector>
#include <wrl.h>

class Window;

struct D32D12_CPU_DESCRIPTOR_HANDLE;

struct ID3D12Fence;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct DXGI_SWAP_CHAIN_DESC;
struct IDXGISwapChain;

struct ID3D12DescriptorHeap;
struct ID3D12Resource;
struct D3D12_RENDER_TARGET_VIEW_DESC;
struct D3D12_CPU_DESCRIPTOR_HANDLE;

struct D3D12_HEAP_PROPERTIES;
struct D3D12_RESOURCE_DESC;
struct D3D12_CLEAR_VALUE;
struct D3D12_DEPTH_STENCIL_VIEW_DESC;

struct IDXGIAdapter;
struct IDXGIOutput;
struct DXGI_MODE_DESC;

struct IDXGIFactory4;
struct ID3D12Device;

class Renderer
{
public:
    Renderer(Window& window);
    ~Renderer() = default;

private:
    void CreateFactory();
    void CreateDevice(enum D3D_FEATURE_LEVEL featureLevel);

    void CreateFence(ID3D12Fence** fence) const;
    void CreateCommandObjects(ID3D12CommandQueue** commandQueue, ID3D12CommandAllocator** commandAllocator, ID3D12GraphicsCommandList** commandList) const;
    void CreateSwapChain(ID3D12CommandQueue* commandQueue, DXGI_SWAP_CHAIN_DESC* swapChainDesc, IDXGISwapChain** swapChain) const;

    UINT GetDescriptorSize(enum D3D12_DESCRIPTOR_HEAP_TYPE type) const;
    UINT GetMsaaQuality(enum DXGI_FORMAT format, UINT sampleCount) const;
    void CreateDescriptorHeap(UINT numDescriptors, enum D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, ID3D12DescriptorHeap** descriptorHeap) const;
    void CreateRenderTargetView(ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const;

    void CreateCommittedResource(D3D12_HEAP_PROPERTIES* heapProperties, enum D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_DESC* resourceDesc, enum D3D12_RESOURCE_STATES initialState, D3D12_CLEAR_VALUE* clearValue, ID3D12Resource** resource) const;
    void CreateDepthStencilView(ID3D12Resource* resource, D3D12_DEPTH_STENCIL_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const;

    UINT GetAdapters(std::vector<IDXGIAdapter*>& adapters) const;
    static UINT GetAdapterOutputs(IDXGIAdapter* adapter, std::vector<IDXGIOutput*>& outputs);
    static UINT GetOutputDisplayModes(IDXGIOutput* output, enum DXGI_FORMAT format, UINT flags, std::vector<DXGI_MODE_DESC>& modes);

    D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferView() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
    Microsoft::WRL::ComPtr<ID3D12Device> m_Device;

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
