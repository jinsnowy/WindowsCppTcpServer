#pragma once

class TcpSocket : public NetworkObjectBase
{
protected:
	SOCKET  mSocket;
	EndPoint mEndPoint;
	
	atomic<bool> mDisposed;
	atomic<bool> mConnected;

protected:
	TcpSocket();
	TcpSocket(SOCKET socket);

public:
	virtual ~TcpSocket();
	SOCKET getHandle() { return mSocket; }
	EndPoint getEndPoint() const { return mEndPoint; }

	void setConnectInfo(const EndPoint& endPoint);
	void setLingerOption(bool enableLingerOption, unsigned short maxTimeWaitSeconds);
	void setSendBufferingOption(int numBufferSize);
	bool isConnected() const noexcept { return mConnected; }

	void close(const char* reason = "Close");
	void dispose(const char* reason = "Dispose");
	void release() { if (isConnected()) { close("release"); } dispose("release"); }

protected:
	virtual void onDisposed();
	virtual void onClose();

	void setNonblocking(bool bNonblock);
	void setConnected(bool bConnect) noexcept;
private:
	bool checkDisposed(const char* action);
};