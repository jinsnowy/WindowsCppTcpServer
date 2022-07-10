#pragma once

#include "EventSelect.h"
#include "ConnectorFactory.h"

namespace pkt
{
	class Packet;
}

class Logger;
class IConnector;
class TcpConnector : public EventSelect
{
	using PacketPtr = std::shared_ptr<pkt::Packet>;
public:
	TcpConnector(ConnectorFactory connectorFactory);

	virtual ~TcpConnector();

	virtual bool initialize() override;

	virtual void shutdown() override;

	bool connect(const EndPoint& endPoint);
	bool reconnect(const EndPoint& endPoint);

	void connectAsync(const EndPoint& endPoint);
	void reconnectAsync(const EndPoint& endPoint);

	bool isConnected() const;

	void handleNetworkEvent();

	bool writeMessage(const PacketPtr& packet);
	bool writeMessage(const std::vector<PacketPtr>& packets);
protected:
	virtual void onIoRead() override;
	virtual void onIoConnectedSucessfully() override;
	virtual void onIoConnectedFailed() override;
	virtual void onIoDisconnected() override;
	virtual SOCKET getSocketHandle() override;

private:
	EndPoint  mTargetEndPoint;
	ConnectorFactory mConnectorFactory;
	std::unique_ptr<IConnector> mConnector;

	std::mutex mSync;
	std::vector<PacketPtr> mPacketQue;
};

