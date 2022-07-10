#pragma once

class IListener
{
public:
	virtual ~IListener() = 0;

	virtual SOCKET getHandle() = 0;

	virtual bool listen(const uint16& port, int backlog) = 0;

	virtual bool accept(SOCKET& hAcceptSocket, SOCKADDR_IN* acceptedSocketAddress) = 0;

	virtual bool accept(SOCKET hAcceptSocket) = 0;

	virtual  bool onAccept(SOCKET hAcceptSocket, SOCKADDR_IN* acceptEndPoint) = 0;
};