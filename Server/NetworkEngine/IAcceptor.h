#pragma once
#include "TcpActiveSocket.h"

class IAcceptor : public TcpActiveSocket
{
protected:
	IAcceptor() = default;
	IAcceptor(SOCKET socket) : TcpActiveSocket(socket) {};

public:
	virtual ~IAcceptor() = 0;

	virtual bool onRead(DWORD) = 0;

	virtual bool onWrite(DWORD)  = 0;

	virtual bool read() = 0;
};