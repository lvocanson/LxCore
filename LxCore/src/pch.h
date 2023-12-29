#pragma once

// Windows API
#include <Windows.h>
#include <wrl.h>        // Microsoft ComPtr

// DirectX
#include <dxgi1_6.h>    // DirectX Graphics Infrastructure
#include <d3d12.h>      // DirectX 12

// Standard Library
#include <vector>
#include <sstream>

// Debugging
#if (defined(DEBUG) || defined(_DEBUG))

#define LxAssert(condition, message)\
    if (!(condition))\
    {\
        std::ostringstream ss;\
        ss << message;\
        MessageBoxA(nullptr, ss.str().c_str(), "Assertion Failed!", MB_OK | MB_ICONERROR);\
        __debugbreak();\
    } static_assert(true, "")

#else

#define LxAssert(condition, message) condition

#endif

#define LxHrAssert(hr, message) LxAssert(SUCCEEDED(hr), message)
