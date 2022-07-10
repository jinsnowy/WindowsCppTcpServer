#pragma once

#include "IAcceptor.h"

class TcpAcceptorSocket : public IAcceptor
{
private:
    TcpAcceptorSocket();
public:
    TcpAcceptorSocket(SOCKET socket);
    virtual ~TcpAcceptorSocket();

public:
    // accpetor interface
    virtual bool onRead(DWORD dwSize) override { return onReadInternal(dwSize); }
    virtual bool onWrite(DWORD dwSize) override { return onWriteInternal(dwSize); };
    virtual bool read() override { return readAsync(); };

    virtual std::string toString() const override;
private:
    bool onReadInternal(DWORD recvBytes);
    bool onWriteInternal(DWORD sentBytes);

protected:
    virtual void onClose() override;
};