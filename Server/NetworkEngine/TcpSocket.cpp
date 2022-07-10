#include "pch.h"
#include "TcpSocket.h"

TcpSocket::TcpSocket()
	:
	mSocket(::WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)),
	mDisposed(false),
	mConnected(false)
{	
}

TcpSocket::TcpSocket(SOCKET socket)
	:
	mSocket(socket),
	mDisposed(false),
	mConnected(false)
{
}

TcpSocket::~TcpSocket()
{
	dispose("destructor");
}

void TcpSocket::setConnectInfo(const EndPoint& endPoint)
{
	mEndPoint = endPoint;
	setConnected(true);
}

bool TcpSocket::checkDisposed(const char* action)
{
	if (mDisposed == 1)
	{
		LOG_INFO(mLogger, "%s already disposed when %s, connect(%s)", toString().c_str(), action, get_bool_str(mConnected));
		return true;
	}

	return false;
}

void TcpSocket::setLingerOption(bool enableLingerOption, unsigned short maxTimeWaitSeconds)
{
	LINGER lingerStruct = {};
	lingerStruct.l_onoff = (enableLingerOption) ? 1 : 0;
	lingerStruct.l_linger = maxTimeWaitSeconds;

	if (check_sock_error(::setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)&lingerStruct, sizeof(lingerStruct))))
	{
		LOG_ERROR(mLogger, "%s failed to set linger option (%s), maxWaitTimeSec(%d)", toString().c_str(), get_bool_str(enableLingerOption), (int)maxTimeWaitSeconds);
	}
}

void TcpSocket::setSendBufferingOption(int numBufferSize)
{
	if (numBufferSize <= 10)
	{
		LOG_ERROR(mLogger, "%s failed to set sendBuffering option too small : %d", numBufferSize);
		return;
	}

	if (check_sock_error(::setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char*)&numBufferSize, sizeof(numBufferSize))))
	{
		LOG_ERROR(mLogger, "%s failed to set sendBuffering option : %d", numBufferSize);
	}
}

void TcpSocket::close(const char* action)
{
	if (checkDisposed(action))
		return;

	bool expected = true;
	if (mConnected.compare_exchange_strong(expected, false) == true)
	{
		LOG_INFO(mLogger, "closing by %s", action);

		onClose();

		if (check_sock_error(::shutdown(mSocket, SD_BOTH)))
		{
			LOG_ERROR(mLogger, "shutdown socket failed : %s", get_last_err_msg());
		}
	}
}

void TcpSocket::dispose(const char* action)
{
	if (checkDisposed(action))
		return;

	bool expected = false;
	if (mDisposed.compare_exchange_strong(expected, true) == true)
	{
		onDisposed();

		if (mConnected)
		{
			close(action);
		}

		if (check_sock_error(::closesocket(mSocket)))
		{
			LOG_ERROR(mLogger, "close socket failed : %s", get_last_err_msg());
		}
	}
}

void TcpSocket::onDisposed()
{
}

void TcpSocket::onClose()
{
}

void TcpSocket::setNonblocking(bool bNonblock)
{
	u_long mode = bNonblock ? 1 : 0;
	if (check_sock_error(::ioctlsocket(mSocket, FIONBIO, &mode)))
	{
		LOG_FATAL(mLogger, "socket non blocking mode set %d failed %s", mode, toString().c_str());
	}
}

void TcpSocket::setConnected(bool bConnect) noexcept
{
	mConnected.exchange(bConnect);
	LOG_INFO(mLogger, "set to connect %s", get_bool_str(mConnected));
}