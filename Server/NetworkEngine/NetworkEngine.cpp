#include "pch.h"
#include "NetworkEngine.h"

NetworkEngine::NetworkEngine()
	:
	logger(Logger::getInstance()),
	ServerIocpThreadNum(std::thread::hardware_concurrency()),
	ConnectTimeoutMs(10000),
	WriteBufferBlockNum(4),
	IoDataBufferNum(8192),
	CloseOnPacketProtocolNotFound(false)
{
}

bool NetworkEngine::initialize()
{
	WSADATA wsaData = {};
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		LOG_ERROR(logger, "WinSock Init Failed");
		return false;
	}

	return true;
}

NetworkEngine::~NetworkEngine()
{
	WSACleanup();
}
