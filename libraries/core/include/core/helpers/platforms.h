#pragma once

#if defined(__linux__)
#define P_LINUX
#endif

#if defined(__APPLE__)
#define P_MACOS
#endif

#if defined(_WIN32)
#define P_WINDOWS
#endif
