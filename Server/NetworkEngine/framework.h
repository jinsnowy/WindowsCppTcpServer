#pragma once

#undef UNICODE

#define _WIN32_WINNT 0x0A00 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define NOMINMAX

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <rpc.h>


// #define BOOST_SUPPORT
#ifdef BOOST_SUPPORT
#include <boost/stacktrace.hpp>

#ifdef DEBUG
#pragma comment(lib, "libboost_stacktrace_windbg-vc143-mt-gd-x64-1_79.lib")
#endif

#ifdef NDEBUG
#pragma comment(lib, "libboost_stacktrace_windbg-vc143-mt-x64-1_79.lib")
#endif

#endif