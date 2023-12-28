#pragma once

// Include this file before using any Windows API.

#if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) || defined(__CYGWIN__)
#error "Unsupported platform. Windows is the only supported platform for this library."
#endif
