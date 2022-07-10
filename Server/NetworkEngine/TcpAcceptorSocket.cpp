#include "pch.h"
#include "TcpAcceptorSocket.h"
#include "NetworkStream.h"
#include "IoDataBuffer.h"
#include "Packet.h"
#include "PacketHandler.h"

using namespace pkt;

TcpAcceptorSocket::TcpAcceptorSocket()
{
}

TcpAcceptorSocket::TcpAcceptorSocket(SOCKET socket)
	:
	IAcceptor(socket)
{
}

TcpAcceptorSocket::~TcpAcceptorSocket()
{
}

std::string TcpAcceptorSocket::toString() const
{
	return Format::format("TcpAcceptorSocket : connected(%s), disposed(%s), end point(%s)", get_bool_str(mConnected), get_bool_str(mDisposed), mEndPoint.toString().c_str());
}

bool TcpAcceptorSocket::onReadInternal(DWORD recvBytes)
{
	if (recvBytes == 0)
	{
		LOG_ERROR(mLogger, "recv zero bytes");
		return false;
	}

	if (!mRdBuffer->checkRecvBytes(recvBytes))
	{
		LOG_ERROR(mLogger, "check recv bytes failed : %s", mRdBuffer->checkRecvBytesErrMsg(recvBytes).c_str());
		return false;
	}

	int offset = 0; int recvInt = recvBytes;
	int typeCode = -1, pktSize = 0;
	std::vector<unsigned char> byteStream;
	while (mRdBuffer->readCodeBytes(typeCode, pktSize, byteStream))
	{
		const pkt::PacketHandlerCallback* callback;
		if (!getPacketHandlerCallback(typeCode, &callback))
		{
			LOG_ERROR(mLogger, "packet handler not found for typecode (%d)", typeCode);
			return !NetworkEngine::getInstance()->getCloseOnPacketProtocolNotFound();
		}

		std::shared_ptr<pkt::Packet> packet = nullptr;
		if (!mPktManager->crackMessage(typeCode, byteStream, packet))
		{
			LOG_ERROR(mLogger, "crack message failed for typecode (%d)", typeCode);
			return false;
		}

		(*callback)(packet);
	}

	return true;
}

bool TcpAcceptorSocket::onWriteInternal(DWORD bytesSent)
{
	return true;
}

void TcpAcceptorSocket::onClose()
{
	LOG_INFO(mLogger, "%s connection dropped", toString().c_str());

	setLingerOption(true, 0);
}