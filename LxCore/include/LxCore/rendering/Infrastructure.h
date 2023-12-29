#pragma once
#include <vector>
#include <wrl.h>

struct IDXGIAdapter;
struct IDXGIOutput;
struct DXGI_MODE_DESC;
struct IDXGIFactory4;

class Infrastructure
{
public:
    Infrastructure();
    ~Infrastructure() = default;

    UINT GetAdapters(std::vector<IDXGIAdapter*>& adapters) const;
    static UINT GetAdapterOutputs(IDXGIAdapter* adapter, std::vector<IDXGIOutput*>& outputs);
    static UINT GetOutputDisplayModes(IDXGIOutput* output, enum DXGI_FORMAT format, UINT flags, std::vector<DXGI_MODE_DESC>& modes);
    
private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
};
