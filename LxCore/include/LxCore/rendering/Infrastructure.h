#pragma once
#include <vector>
#include <wrl.h>

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

struct IDXGIAdapter;
struct IDXGIOutput;
struct DXGI_MODE_DESC;

struct IDXGIFactory4;
struct ID3D12Device;

class Infrastructure
{
public:
    Infrastructure(enum D3D_FEATURE_LEVEL featureLevel);
    ~Infrastructure() = default;

    void CreateFence(ID3D12Fence** fence) const;
    void CreateCommandObjects(ID3D12CommandQueue** commandQueue, ID3D12CommandAllocator** commandAllocator, ID3D12GraphicsCommandList** commandList) const;
    void CreateSwapChain(ID3D12CommandQueue* commandQueue, DXGI_SWAP_CHAIN_DESC* swapChainDesc, IDXGISwapChain** swapChain) const;

    UINT GetDescriptorSize(enum D3D12_DESCRIPTOR_HEAP_TYPE type) const;
    UINT GetMsaaQuality(enum DXGI_FORMAT format, UINT sampleCount) const;
    void CreateDescriptorHeap(UINT numDescriptors, enum D3D12_DESCRIPTOR_HEAP_TYPE type, bool shaderVisible, ID3D12DescriptorHeap** descriptorHeap) const;
    void CreateRenderTargetView(ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const;

    UINT GetAdapters(std::vector<IDXGIAdapter*>& adapters) const;
    static UINT GetAdapterOutputs(IDXGIAdapter* adapter, std::vector<IDXGIOutput*>& outputs);
    static UINT GetOutputDisplayModes(IDXGIOutput* output, enum DXGI_FORMAT format, UINT flags, std::vector<DXGI_MODE_DESC>& modes);

private:
    void CreateFactory();
    void CreateDevice(enum D3D_FEATURE_LEVEL featureLevel);

private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
    Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
};
