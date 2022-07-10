#include "pch.h"
#include "TcpConnectorSocket.h"
#include "NetworkEngine.h"

TcpConnectorSocket::TcpConnectorSocket()
{
}

TcpConnectorSocket::~TcpConnectorSocket()
{
}

bool TcpConnectorSocket::connect(const EndPoint& endPoint)
{
	if (checkConnected())
	{
		return false;
	}

	if (check_sock_error(::WSAConnect(mSocket, endPoint.getData(), (int)endPoint.getSize(), nullptr, nullptr, nullptr, nullptr)))
	{
		bool was_pending = would_block(WSAGetLastError());
		if (!was_pending)
		{
			LOG_ERROR(mLogger, "conenct to %s failed : %d, %s", endPoint.toString().c_str(), WSAGetLastError(), get_last_err_msg());
			return false;
		}
	}

	int count = 0;
	int maxCount = NetworkEngine::getInstance()->getConnectTimeoutMs() / 100;
	while (!isConnected() && count++ < maxCount)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return isConnected();
}

void TcpConnectorSocket::connectAsync(const EndPoint& endPoint)
{
	if (checkConnected())
	{
		return;
	}

	if (check_sock_error(::WSAConnect(mSocket, endPoint.getData(), (int)endPoint.getSize(), nullptr, nullptr, nullptr, nullptr)))
	{
		bool was_pending = would_block(WSAGetLastError());
		if (was_pending)
		{
			return;
		}

		LOG_ERROR(mLogger, "conenct to %s failed : %d, %s", endPoint.toString().c_str(), WSAGetLastError(), get_last_err_msg());
	}
}

bool TcpConnectorSocket::reconnect(const EndPoint& endPoint)
{
	if (isConnected())
	{
		close("reconnect");
	}

	return connect(endPoint);
}

void TcpConnectorSocket::reconnectAsync(const EndPoint& endPoint)
{
	if (isConnected())
	{
		close("reconnect");
	}

	return connectAsync(endPoint);
}

std::string TcpConnectorSocket::toString() const
{
	return Format::format("TcpConnectorSocket : connected(%s), disposed(%s), end point(%s)", get_bool_str(mConnected), get_bool_str(mDisposed), mEndPoint.toString().c_str());
}

void TcpConnectorSocket::onDisconnect()
{
	LOG_INFO(mLogger, "on disconnected");

	setConnected(false);
}

bool TcpConnectorSocket::read(std::vector<std::shared_ptr<pkt::Packet>>& outPackets)
{
	try 
	{
		if (!TcpActiveSocket::read(outPackets))
		{
			close("read failed");
			return false;
		}
	}
	catch (std::exception e)
	{
		LOG_ERROR(mLogger, "read packets failed : %s", e.what());
		close(Format::format("exception=%s", e.what()).c_str());
		return false;
	}
	
	return true;
}

bool TcpConnectorSocket::checkConnected()
{
	if (isConnected() == true)
	{
		LOG_ERROR(mLogger, "connected already");
		return true;
	}

	return false;
}