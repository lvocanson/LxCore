#pragma once
#include <vector>
#include <wrl.h>

struct ID3D12Fence;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;

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

    UINT GetAdapters(std::vector<IDXGIAdapter*>& adapters) const;
    static UINT GetAdapterOutputs(IDXGIAdapter* adapter, std::vector<IDXGIOutput*>& outputs);
    static UINT GetOutputDisplayModes(IDXGIOutput* output, enum DXGI_FORMAT format, UINT flags, std::vector<DXGI_MODE_DESC>& modes);

private:
    void CreateFactory();
    void CreateDevice(enum D3D_FEATURE_LEVEL featureLevel);
    void GetDescriptorSizes();
    void Get4xMsaaQualityLevels(enum DXGI_FORMAT format);

private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
    Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
    UINT m_RtvDescriptorSize, m_DsvDescriptorSize, m_CbvSrvUavDescriptorSize;
    UINT m_4xMsaaQuality;
};
