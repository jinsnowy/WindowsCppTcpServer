#include "pch.h"
#include "TcpListenerSocket.h"
#include "TcpActiveSocket.h"

TcpListenerSocket::TcpListenerSocket()
    :
    mListening(false)
{
    setNonblocking(false);
    //mAcceptIoData.setBuffer(mAcceptBuffer, sizeof(mAcceptBuffer));
}

TcpListenerSocket::~TcpListenerSocket()
{
    stop();
}

bool TcpListenerSocket::listen(const uint16& port, int backlog)
{
    if (isListening())
    {
        LOG_ERROR(mLogger, "TcpListenerSocket already listening");
        return false;
    }

    if (!mAcceptExFn.initialize(mSocket))
    {
        LOG_ERROR(mLogger, "initialize accept fn failed : %s", get_last_err_msg());
        return false;
    }

    if (!mGetAcceptExSockAddrsFn.initialize(mSocket))
    {
        LOG_ERROR(mLogger, "initialize update accept context fn failed");
        return false;
    }

    EndPoint endPoint = EndPoint("127.0.0.1", port);
    if (check_sock_error(::bind(mSocket, endPoint.getData(), (int)endPoint.getSize())))
    {
        LOG_ERROR(mLogger, "bind error : %s, end point : %s ", get_last_err_msg(), endPoint.toString().c_str());
        return false;
    }

    if (check_sock_error(::listen(mSocket, backlog)))
    {
        LOG_ERROR(mLogger, "listen error : %s, end point : %s ", get_last_err_msg(), endPoint.toString().c_str());
        return false;
    }

    mListening = true;
    setConnectInfo(endPoint);

    return true;
}

bool TcpListenerSocket::accept(SOCKET& hAcceptSocket, SOCKADDR_IN* acceptedSocketAddress)
{
    static int szSockAddr = sizeof(SOCKADDR);

    hAcceptSocket = ::accept(getHandle(), (SOCKADDR*)acceptedSocketAddress, &szSockAddr);

    if (hAcceptSocket == INVALID_SOCKET)
    {
        LOG_ERROR(mLogger, "accept failed : %s", get_last_err_msg());
        return false;
    }

    return true;
}

bool TcpListenerSocket::accept(SOCKET hAcceptSocket)
{
    //ZeroMemory(mAcceptBuffer, sizeof(mAcceptBuffer));

    //DWORD notUsed = 0;
    //int res = mAcceptExFn.acceptEx(
    //    mSocket, // listening socket
    //    hAcceptSocket, // accepting socket (in future)
    //    mAcceptIoData.getBuffer(),
    //    mAcceptIoData.getBufferLength() - (2 * (sizeof(SOCKADDR_STORAGE) + 16)),
    //    sizeof(SOCKADDR_STORAGE) + 16,
    //    sizeof(SOCKADDR_STORAGE) + 16,
    //    &notUsed,
    //    mAcceptIoData.getOverlapped());

    //DWORD errorID = WSAGetLastError();
    //if (res == SOCKET_ERROR && errorID != ERROR_IO_PENDING)
    //{
    //    LOG_ERROR(mLogger, "accept ex failed %s", get_last_err_msg());

    //    return false;
    //}

    //LOG_INFO(mLogger, "accept pending ...");

    // return true;

    return false;
}

bool TcpListenerSocket::onAccept(SOCKET acceptedSocket, SOCKADDR_IN* acceptedSocketAddress)
{
   /* if (check_sock_error(setsockopt(acceptedSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&mSocket, sizeof(mSocket))))
    {
        LOG_ERROR(mLogger, "SO_UPDATE_ACCEPT_CONTEXT failed : %s", get_last_err_msg());
        return false;
    }

    SOCKADDR_IN* RemoteAddress = NULL;
    INT			 LocalAddressLength = 0;
    INT			 RemoteAddressLength = 0;

    mGetAcceptExSockAddrsFn.getAcceptExSockAddrs(
                            mAcceptIoData.getBuffer(),
                            mAcceptIoData.getBufferLength() - 2 * (sizeof(SOCKADDR_IN) + 16),
                            sizeof(SOCKADDR_IN) + 16,
                            sizeof(SOCKADDR_IN) + 16,
                            (sockaddr**)&acceptedSocketAddress,
                            &LocalAddressLength,
                            (sockaddr**)&RemoteAddress,
                            &RemoteAddressLength);

    if (acceptedSocketAddress == nullptr || RemoteAddress == nullptr)
    {
        LOG_ERROR(mLogger, "GetAcceptExSockAddrsFn failed : %s", get_last_err_msg());
        return false;
    }

    return true;*/

    return false;
}

void TcpListenerSocket::stop()
{
    dispose("TcpListenerSocket::Stop");
    mListening = false;
}

std::string TcpListenerSocket::toString() const
{
	return Format::format("TcpListenerSocket : Connected(%s), Disposed(%s), EndPoint(%s)", get_bool_str(mConnected), get_bool_str(mDisposed), mEndPoint.toString().c_str());
}