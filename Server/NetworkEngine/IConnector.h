#pragma once
#include "TcpActiveSocket.h"

namespace pkt
{
	class Packet;
}

class IConnector : public TcpActiveSocket
{
public:
	virtual ~IConnector() = 0;

	virtual bool connect(const EndPoint& endPoint) = 0;
	virtual bool reconnect(const EndPoint& endPoint) = 0;
	virtual void connectAsync(const EndPoint& endPoint) = 0;
	virtual void reconnectAsync(const EndPoint& endPoint) = 0;

	virtual void onDisconnect() = 0;
	virtual bool read(std::vector<std::shared_ptr<pkt::Packet>>& outPackets) = 0;
};