#pragma once
#include "TcpSocket.h"
#include "IListener.h"
using AcceptCallback = std::function<void(const SOCKET& sock, const EndPoint& point)>;

class TcpActiveSocket;
class TcpListenerSocket : public TcpSocket, public IListener
{
private:
    atomic<bool>    mListening;

public:
    TcpListenerSocket();

    virtual ~TcpListenerSocket();

    bool isListening() const { return mListening; }

    SOCKET getHandle() override { return mSocket; };

    bool listen(const uint16& port, int backlog = SOMAXCONN) override;

    bool accept(SOCKET& hAcceptSocket, SOCKADDR_IN* acceptedSocketAddress) override;

    bool accept(SOCKET hAcceptSocket) override;

    bool onAccept(SOCKET hAcceptSocket, SOCKADDR_IN* acceptEndPoint) override;

    void stop();

    std::string toString() const override;

private:
    struct AcceptExFunctor
    {
        LPFN_ACCEPTEX acceptEx;

        AcceptExFunctor() : acceptEx(nullptr) {}

        bool initialize(SOCKET hListenSocket)
        {
            DWORD iBytes = 0;
            UUID uuid = WSAID_ACCEPTEX;
            if (check_sock_error(WSAIoctl(hListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &uuid, sizeof(UUID),
                &acceptEx,
                sizeof(acceptEx),
                &iBytes,
                nullptr,
                nullptr)))
            {
                return false;
            }

            return true;
        }
    };

    struct GetAcceptExSockAddrsFunctor
    {
        LPFN_GETACCEPTEXSOCKADDRS getAcceptExSockAddrs;

        GetAcceptExSockAddrsFunctor() {}

        bool initialize(SOCKET hListenSocket)
        {
            DWORD iBytes = 0;
            UUID uuid = UUID(WSAID_GETACCEPTEXSOCKADDRS);
            if (check_sock_error(WSAIoctl(hListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                &uuid, sizeof(UUID),
                &getAcceptExSockAddrs,
                sizeof(getAcceptExSockAddrs),
                &iBytes,
                nullptr,
                nullptr)))
            {
                return false;
            }

            return true;
        }
    };

    AcceptExFunctor mAcceptExFn;
    GetAcceptExSockAddrsFunctor mGetAcceptExSockAddrsFn;
};