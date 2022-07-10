#pragma once
#include "IListener.h"

class IAcceptor;
class IoDataBuffer;
class ThreadPool;
class Iocp
{
private:
	struct CWorkerQueueItem
	{
		IAcceptor* client;
		DWORD	   recvBytes;

		CWorkerQueueItem(IAcceptor* clientIn, const DWORD& recvBytesIn) : client(clientIn), recvBytes(recvBytesIn) {}

		virtual ~CWorkerQueueItem() = default;
	};
private:
	HANDLE					   mCleanupEvent;
	HANDLE					   mIocpHandle;
	IoDataBuffer*			   mBufferMgr;
	IListener*				   mNetworkHandler;
	
	bool					   mExitFlag;
	std::mutex				   mSync;
	std::vector<std::thread>   mIocpWorkers;
	std::unordered_map<uint64, std::unique_ptr<IAcceptor>> mManagedAcceptors;

	std::thread					mAcceptWorker;
	std::unique_ptr<IAcceptor>  mAcceptorPlaceHolder;
	std::function<IAcceptor*(SOCKET)> mAcceptorFactory;
public:
	Iocp(std::function<IAcceptor*(SOCKET)> acceptorFactory);

	virtual ~Iocp();

	virtual bool initialize();

	virtual void shutdown();

protected:
	Logger* mLogger;
	bool initiateAccept(IListener* networkHandler);
	void waitForCleanup();

private:
	void cleanup();
	bool onAccept(std::unique_ptr<IAcceptor>& client);
	void onNewClient(std::unique_ptr<IAcceptor> client);
	void onLeaveClient(IAcceptor* client);

private:
	DWORD WINAPI iocpWorkerThread();
};