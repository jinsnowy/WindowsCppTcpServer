#include "pch.h"
#include "TcpActiveSocket.h"
#include "NetworkStream.h"
#include "IoDataBuffer.h"
#include "PacketHandler.h"

using namespace pkt;

TcpActiveSocket::TcpActiveSocket()
	:
	TcpSocket(),
	mRdBuffer(std::make_unique<ReadStream>()),
	mWrBuffer(std::make_unique<WriteStream>()),
	mIoBuf(IoDataBuffer::getInstance()),
	mPktManager(PacketManager::getInstance()),
	mPacketHandler(nullptr)
{
	setNonblocking(true);
}

TcpActiveSocket::TcpActiveSocket(SOCKET socket)
	:
	TcpSocket(socket),
	mRdBuffer(std::make_unique<ReadStream>()),
	mWrBuffer(std::make_unique<WriteStream>()),
	mIoBuf(IoDataBuffer::getInstance()),
	mPktManager(PacketManager::getInstance()),
	mPacketHandler(nullptr)
{
	setNonblocking(true);
}

TcpActiveSocket::~TcpActiveSocket()
{
}

std::string TcpActiveSocket::toString() const
{
	return Format::format("TcpActiveSocket : connected(%s), disposed(%s), end point(%s)", get_bool_str(mConnected), get_bool_str(mDisposed), mEndPoint.toString().c_str());
}

void TcpActiveSocket::setPacketHandler(IPacketHandler* handler)
{
	mPacketHandler.reset(handler);
}

bool TcpActiveSocket::write(const std::shared_ptr<pkt::Packet>& packet)
{
	if (isConnected())
	{
		return writeAsync(packet);
	}
	else
	{
		LOG_ERROR(mLogger, "write packet on disconnected");
		return false;
	}
}

bool TcpActiveSocket::read(std::vector<std::shared_ptr<pkt::Packet>>& packets)
{
	packets.clear();
	mRdBuffer->rotate();
	auto buffer = mIoBuf->getNext(IO_OPERATION::READ, mRdBuffer->getCurPtr(), mRdBuffer->available());

	DWORD recvBytes = 0, flags = 0;
	if (check_sock_error(::WSARecv(mSocket, buffer->getWsaBuf(), 1, &recvBytes, &flags, buffer->getOverlapped(), nullptr)))
	{
		DWORD dwError = WSAGetLastError();
		if (dwError == WSA_IO_PENDING)
		{
			LOG_INFO(mLogger, "socket %s recv io pending ...", toString().c_str());
			return true;
		}
		else if (dwError == WSA_OPERATION_ABORTED)
			LOG_INFO(mLogger, "socket %s closed.", toString().c_str());
		else
			LOG_ERROR(mLogger, "socket recv error %s", get_last_err_msg());

		return false;
	}

	if (recvBytes == 0)
	{
		LOG_ERROR(mLogger, "no data to read");
		return false;
	}

	if (!mRdBuffer->checkRecvBytes(recvBytes))
	{
		LOG_ERROR(mLogger, "check recv bytes failed : %s", mRdBuffer->checkRecvBytesErrMsg(recvBytes).c_str());
		return false;
	}

	int typeCode = -1, pktSize = 0;
	std::vector<unsigned char> byteStream;
	while (mRdBuffer->readCodeBytes(typeCode, pktSize, byteStream))
	{
		std::shared_ptr<pkt::Packet> packet;
		if (!mPktManager->crackMessage(typeCode, byteStream, packet))
		{
			LOG_ERROR(mLogger, "crack message failed for typecode (%d)", typeCode);
			return false;
		}

		packets.push_back(std::move(packet));
	}

	return true;
}

bool TcpActiveSocket::readAsync()
{
	mRdBuffer->rotate();
	auto buffer = mIoBuf->getNext(IO_OPERATION::READ, mRdBuffer->getCurPtr(), mRdBuffer->available());

	DWORD flags = 0;
	DWORD recvBytes = 0;
	if (check_sock_error(::WSARecv(mSocket, buffer->getWsaBuf(), 1, &recvBytes, &flags, buffer->getOverlapped(), nullptr))
		&& was_io_pending(WSAGetLastError()) == false)
	{
		DWORD dwLastError = WSAGetLastError();
		if (dwLastError == WSA_OPERATION_ABORTED || dwLastError == WSAECONNRESET)
			LOG_INFO(mLogger, "socket %s closed.", toString().c_str());
		else
			LOG_ERROR(mLogger, "socket recv error %d, %s", dwLastError, get_last_err_msg());

		return false;
	}

	return true;
}

bool TcpActiveSocket::getPacketHandlerCallback(int typeCode, const pkt::PacketHandlerCallback** callback)
{
	if (mPacketHandler == nullptr)
	{
		LOG_ERROR(mLogger, "null handler");
		return false;
	}

	if (!mPacketHandler->getHandler(typeCode, callback))
	{
		LOG_INFO(mLogger, "no handler registered for typecode (%d)", typeCode);
		return false;
	}

	return true;
}

bool TcpActiveSocket::writeAsync(const std::shared_ptr<Packet>& packet)
{
	int packetSize = packet->getSize();
	if (packetSize > MAX_PACKET_SIZE)
	{
		LOG_ERROR(mLogger, "packet is too big %d", packetSize);
		close("packet size is too big");
		return false;
	}

	unsigned char* bufferPtr = nullptr;
	if (!mWrBuffer->getWriteBuffer(packetSize, &bufferPtr))
	{
		LOG_ERROR(mLogger, "out of memory in write buffer %s", mWrBuffer->getVerbose().c_str());
		close("out of memory in write buffer");
		return false;
	}

	const auto& packetBytes = packet->serialize();
	if ((int)packetBytes.size() != packetSize)
	{
		LOG_ERROR(mLogger, "invalid packet size %d != %d", (int)packetBytes.size(), packetSize);
		close("invalid packet size");
		return false;
	}

	memcpy(bufferPtr, packetBytes.data(), packetSize);

	auto buffer = mIoBuf->getNext(IO_OPERATION::WRITE, (char*)bufferPtr, packetSize);

	DWORD writeBytes = 0, flags = 0;
	if (check_sock_error(::WSASend(mSocket, buffer->getWsaBuf(), 1, &writeBytes, flags, buffer->getOverlapped(), nullptr))
		&& was_io_pending(WSAGetLastError()) == false)
	{
		if (WSAGetLastError() == WSA_OPERATION_ABORTED)
			LOG_INFO(mLogger, "socket %s closed.", toString().c_str());
		else
			LOG_ERROR(mLogger, "socket write error %s", get_last_err_msg());

		return false;
	}

	return true;
}
