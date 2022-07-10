#include "pch.h"
#include "Iocp.h"
#include "TcpListener.h"
#include "TcpListenerSocket.h"
#include "TcpAcceptorSocket.h"
#include "Packet.h"

using namespace pkt;

TcpListener::TcpListener(AcceptorFactory acceptorFactory)
    :
    Iocp([this](SOCKET socket) { return onCreateAcceptor(socket); }),
    mAcceptorFactory(acceptorFactory),
    mNetworkHandler(std::make_unique<TcpListenerSocket>())
{
}

void TcpListener::start(const uint16& port, int backlog)
{
    if (!mNetworkHandler->listen(port, backlog))
    {
        return;
    }

    if (!initiateAccept(mNetworkHandler.get()))
    {
        return;
    }

    LOG_INFO(mLogger, "server start on %s ...", mNetworkHandler->getEndPoint().toString().c_str());

    waitForCleanup();
}

void TcpListener::shutdown()
{
    Iocp::shutdown();

    mNetworkHandler->stop();
}

IAcceptor* TcpListener::onCreateAcceptor(SOCKET socket)
{
    return mAcceptorFactory(socket);
}
