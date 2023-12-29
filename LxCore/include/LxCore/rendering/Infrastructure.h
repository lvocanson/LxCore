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
    Infrastructure() = default;
    ~Infrastructure() = default;
    bool Init();
    const wchar_t* GetLastErrorMessage() { return m_LastErrorMessage; }

    UINT GetAdapters(std::vector<IDXGIAdapter*>& adapters) const;
    static UINT GetAdapterOutputs(IDXGIAdapter* adapter, std::vector<IDXGIOutput*>& outputs);
    static UINT GetOutputDisplayModes(IDXGIOutput* output, enum DXGI_FORMAT format, std::vector<DXGI_MODE_DESC>& modes);

private:
    const wchar_t* m_LastErrorMessage = nullptr;
    Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
};
