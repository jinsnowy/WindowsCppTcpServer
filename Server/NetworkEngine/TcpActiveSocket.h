#pragma once
#include "TcpSocket.h"
#include "ISender.h"
#include "Packet.h"

namespace pkt
{
    class Packet;
    class PacketManager;
}

class IoDataBuffer;
class ReadStream;
class WriteStream;
class IPacketHandler;
class TcpActiveSocket : public TcpSocket, public ISender
{
protected:
    pkt::PacketManager*            mPktManager;
    IoDataBuffer*                  mIoBuf;
    std::unique_ptr<ReadStream>    mRdBuffer;
    std::unique_ptr<WriteStream>   mWrBuffer;

private:
    std::unique_ptr<IPacketHandler> mPacketHandler;

public:
    TcpActiveSocket();
    TcpActiveSocket(SOCKET socket);

    virtual ~TcpActiveSocket();

    virtual std::string toString() const override;

    void setPacketHandler(IPacketHandler* handler);

    bool write(const std::shared_ptr<pkt::Packet>& packet) override;

    bool read(std::vector<std::shared_ptr<pkt::Packet>>& packets);

    bool getPacketHandlerCallback(int typeCode, const pkt::PacketHandlerCallback** callback);

protected:
    bool writeAsync(const std::shared_ptr<pkt::Packet>& packet);

    bool readAsync();
};
