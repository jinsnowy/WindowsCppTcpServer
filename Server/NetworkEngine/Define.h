#pragma once

#ifndef _DEFINE_H_
#define _DEFINE_H_

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

static inline std::string WSAGetLastErrorMsg()
{
    auto errorMessageID = ::WSAGetLastError();

    char* szMessage = NULL;
    LPSTR messageBuffer = nullptr;

    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    LocalFree(messageBuffer);

	return message;
}

static inline std::string WSAGetLastErrorMsg(int errCode)
{
    char* szMessage = NULL;
    LPSTR messageBuffer = nullptr;

    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    LocalFree(messageBuffer);

    return message;
}

static const char* get_bool_str(const bool& v) { return v ? "true" : "false"; }
// check socket call

#define check_sock_error(exp) ((exp) == SOCKET_ERROR)
#define was_io_pending(err) ((err == WSA_IO_PENDING))
#define would_block(err) ((err == WSAEWOULDBLOCK))
#define safe_close_event(h) if (h != NULL) { WSACloseEvent(h); h = NULL; }
#define safe_close_handle(h) if (h != NULL) { CloseHandle(h); h = NULL; }
#define get_last_err_msg() WSAGetLastErrorMsg().c_str()
#define get_last_err_msg_code(errCode) WSAGetLastErrorMsg(errCode).c_str()

#include "NetworkObjectBase.h"

#endif // !_DEFINE_H_
