#pragma once
#include "IAcceptor.h"
#include "PacketHandler.h"

struct AcceptorFactory
{
	function<IAcceptor*(SOCKET)> acceptorFactory;
	function<IPacketHandler*(IAcceptor* acceptor)> packetHandlerFactory;

	AcceptorFactory(function<IAcceptor* (SOCKET)> acceptorFactoryIn,
		function<IPacketHandler*(ISender* sender)> packetHandlerFactoryIn)
		:
		acceptorFactory(acceptorFactoryIn), packetHandlerFactory(packetHandlerFactoryIn) {}

	IAcceptor* operator()(SOCKET socket) const
	{
		unique_ptr<IAcceptor> acceptor(acceptorFactory(socket));
		acceptor->setPacketHandler(packetHandlerFactory(acceptor.get()));

		return acceptor.release();
	}
};

