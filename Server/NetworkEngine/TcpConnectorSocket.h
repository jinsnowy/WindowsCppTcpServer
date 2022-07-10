#pragma once
#include "IConnector.h"

class TcpConnectorSocket : public IConnector
{
public:
	TcpConnectorSocket();

	virtual ~TcpConnectorSocket();

	virtual bool connect(const EndPoint& endPoint) override;
	virtual bool reconnect(const EndPoint& endPoint) override;
	virtual void connectAsync(const EndPoint& endPoint) override;
	virtual void reconnectAsync(const EndPoint& endPoint) override;
	virtual void onDisconnect() override;
	virtual bool read(std::vector<std::shared_ptr<pkt::Packet>>& outPacket) override;

	virtual std::string toString() const override;
private:
	bool checkConnected();
};