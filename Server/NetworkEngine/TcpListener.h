#pragma once
#include "AcceptorFactory.h"
#include "Iocp.h"

class TcpListenerSocket;
class TcpListener : public Iocp
{
	using IPacketHandler = IPacketHandler;
private:
	std::unique_ptr<TcpListenerSocket> mNetworkHandler;
	AcceptorFactory					   mAcceptorFactory;
	
public:
	TcpListener(AcceptorFactory acceptorFactory);

	void start(const uint16& port, int backlog = SOMAXCONN);

	virtual void shutdown() override;

private:
	IAcceptor* onCreateAcceptor(SOCKET socket);
};

