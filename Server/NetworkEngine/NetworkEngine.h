#pragma once

#include "Singleton.h"
#include "Logger.h"
#include "Xml.h"

class NetworkEngine : public ISingleton<NetworkEngine>
{
	friend class ISingleton<NetworkEngine>;
private:
	Logger* logger;
	int ServerIocpThreadNum;
	int ConnectTimeoutMs;
	int WriteBufferBlockNum;
	int IoDataBufferNum;
	bool CloseOnPacketProtocolNotFound;

private:
	NetworkEngine();

public:
	virtual bool initialize() override;

	GETSET(ServerIocpThreadNum, int);
	GETSET(ConnectTimeoutMs, int);
	GETSET(WriteBufferBlockNum, int);
	GETSET(IoDataBufferNum, int);
	GETSET(CloseOnPacketProtocolNotFound, bool);

	~NetworkEngine();
};