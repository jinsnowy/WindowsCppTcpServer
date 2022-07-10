#pragma once

#include "IConnector.h"
#include "PacketHandler.h"

struct ConnectorFactory
{
	function<IConnector*()> connectorFactory;
	function<IPacketHandler* (IConnector* connector)> packetHandlerFactory;

	ConnectorFactory(function<IConnector*()> acceptorFactoryIn,
		function<IPacketHandler* (ISender* sender)> packetHandlerFactoryIn)
		:
		connectorFactory(acceptorFactoryIn), packetHandlerFactory(packetHandlerFactoryIn) {}

	IConnector* operator()() const
	{
		unique_ptr<IConnector> connector(connectorFactory());
		connector->setPacketHandler(packetHandlerFactory(connector.get()));

		return connector.release();
	}
};

