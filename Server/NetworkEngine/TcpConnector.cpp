#include "pch.h"
#include "TcpConnector.h"
#include "IConnector.h"
#include "Logger.h"
#include "Packet.h"
#include "PacketHandler.h"

TcpConnector::TcpConnector(ConnectorFactory connectorFactory)
	:
	EventSelect(),
	mConnectorFactory(connectorFactory),
	mTargetEndPoint{}
{
}

TcpConnector::~TcpConnector()
{
}

bool TcpConnector::initialize()
{
	mConnector.reset(mConnectorFactory());

	if (!EventSelect::initialize())
	{
		return false;
	}

	return true;
}

void TcpConnector::shutdown()
{
	EventSelect::shutdown();
}

bool TcpConnector::connect(const EndPoint& endPoint)
{
	mTargetEndPoint = endPoint;
	return mConnector->connect(mTargetEndPoint);
}

void TcpConnector::connectAsync(const EndPoint& endPoint)
{
	mTargetEndPoint = endPoint;
	return mConnector->connectAsync(endPoint);
}

bool TcpConnector::reconnect(const EndPoint& endPoint)
{
	mTargetEndPoint = endPoint;
	return mConnector->reconnect(endPoint);
}

void TcpConnector::reconnectAsync(const EndPoint& endPoint)
{
	mTargetEndPoint = endPoint;
	return mConnector->reconnectAsync(endPoint);
}

bool TcpConnector::isConnected() const
{
	return mConnector->isConnected();
}

void TcpConnector::onIoRead()
{
	std::vector<std::shared_ptr<pkt::Packet>> outPackets;
	if (mConnector->read(outPackets))
	{
		std::lock_guard<std::mutex> lk(mSync);

		for (auto& outPacket : outPackets)
		{
			mPacketQue.push_back(std::move(outPacket));
		}
	}
}

void TcpConnector::onIoConnectedSucessfully()
{
	mConnector->setConnectInfo(mTargetEndPoint);
	LOG_INFO(mLogger, "%s connected !!!", mConnector->toString().c_str());
}

void TcpConnector::onIoConnectedFailed()
{
	LOG_INFO(mLogger, "%s connect failed !!!", mConnector->toString().c_str());
}

void TcpConnector::onIoDisconnected()
{
	mConnector->onDisconnect();
	LOG_INFO(mLogger, "%s disconnected !!!", mConnector->toString().c_str());
}

SOCKET TcpConnector::getSocketHandle()
{
	return mConnector != nullptr ? mConnector->getHandle() : INVALID_SOCKET;
}

void TcpConnector::handleNetworkEvent()
{
	std::vector<std::shared_ptr<pkt::Packet>> packets;
	
	{
		std::lock_guard<std::mutex> lk(mSync);
		packets = std::move(mPacketQue);
		mPacketQue.clear();
	}

	const pkt::PacketHandlerCallback* callback;
	for (const auto& packet : packets)
	{
		if (!mConnector->getPacketHandlerCallback(packet->getTypeCode(), &callback))
		{
			LOG_ERROR(mLogger, "cannot find handler callback : %d", packet->getTypeCode());
			continue;
		}


		try 
		{
			(*callback)(packet);
		}
		catch (std::exception e)
		{
			LOG_ERROR(mLogger, "handler exception %s", e.what());
		}
	}
}

bool TcpConnector::writeMessage(const PacketPtr& packet)
{
	return mConnector->write(packet);
}

bool TcpConnector::writeMessage(const std::vector<PacketPtr>& packets)
{
	for (const auto& packet : packets)
	{
		if (!mConnector->write(packet))
		{
			return false;
		}
	}

	return true;
}
