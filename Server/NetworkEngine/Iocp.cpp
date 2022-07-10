#include "pch.h"
#include "Iocp.h"
#include "Logger.h"
#include "IoData.h"
#include "IoDataBuffer.h"
#include "IAcceptor.h"
#include "NetworkEngine.h"

Iocp::Iocp(std::function<IAcceptor*(SOCKET)> acceptorFactory)
	:
	mCleanupEvent(NULL),
	mIocpHandle(NULL),
	mLogger(Logger::getCurrentLogger()),
	mExitFlag(false),
	mBufferMgr(IoDataBuffer::getInstance()),
	mAcceptorFactory(acceptorFactory),
	mNetworkHandler(nullptr)
{
}

Iocp::~Iocp()
{
	shutdown();
}

bool Iocp::initialize()
{
	if (mAcceptorFactory == nullptr)
	{
		LOG_ERROR(mLogger, "AcceptorFactory is null");
		return false;
	}

	mCleanupEvent = WSACreateEvent();

	DWORD iocpThreadNum = NetworkEngine::getInstance()->getServerIocpThreadNum();
	mIocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, iocpThreadNum);
	if (mIocpHandle == NULL)
	{
		LOG_ERROR(mLogger, "create io completion port failed %s", get_last_err_msg());
		return false;
	}
	
	for (DWORD i = 0; i < iocpThreadNum; ++i)
	{
		mIocpWorkers.emplace_back(&Iocp::iocpWorkerThread, this);
	}

	return true;
}

bool Iocp::initiateAccept(IListener* networkHandler)
{
	if (networkHandler == nullptr)
	{
		LOG_ERROR(mLogger, "networkHandler is null");
		return false;
	}

	mNetworkHandler = networkHandler;

	const auto doAccept = [this]()
	{
		SOCKET hAcceptSocket;
		SOCKADDR_IN hAcceptSocketAddress;
		while (!mExitFlag)
		{
			if (!mNetworkHandler->accept(hAcceptSocket, &hAcceptSocketAddress))
			{
				cleanup();
				return;
			}

			std::unique_ptr<IAcceptor> acceptor(mAcceptorFactory(hAcceptSocket));
			acceptor->setConnectInfo(hAcceptSocketAddress);

			onNewClient(std::move(acceptor));
		}
	};

	mAcceptWorker = std::thread(doAccept);
	return true;
}

void Iocp::waitForCleanup()
{
	::WaitForSingleObject(mCleanupEvent, INFINITE);

	LOG_INFO(mLogger, "server cleanup ...");
}

void Iocp::shutdown()
{
	std::lock_guard<std::mutex> lk(mSync);

	for (const auto& pair : mManagedAcceptors)
	{
		pair.second->release();
	}

	mExitFlag = true;
	int workerNum = (int)mIocpWorkers.size();
	for (int i = 0; i < workerNum; ++i)
	{
		PostQueuedCompletionStatus(mIocpHandle, 0, 0, NULL);
	}

	for (int i = 0; i < workerNum; ++i)
	{
		mIocpWorkers[i].join();
	}

	if (mAcceptWorker.joinable())
	{
		mAcceptWorker.detach();
	}

	mIocpWorkers.clear();
	mManagedAcceptors.clear();

	safe_close_event(mCleanupEvent);
	safe_close_handle(mIocpHandle);
}

void Iocp::cleanup()
{
	WSASetEvent(mCleanupEvent);
}

bool Iocp::onAccept(std::unique_ptr<IAcceptor>& client)
{
	if (mNetworkHandler == nullptr)
	{
		LOG_ERROR(mLogger, "networkHandler is null");
		return false;
	}

	SOCKADDR_IN remoteAdress;
	if (!mNetworkHandler->onAccept(mAcceptorPlaceHolder->getHandle(), &remoteAdress))
	{
		return false;
	}

	mAcceptorPlaceHolder->setConnectInfo(remoteAdress);

	LOG_INFO(mLogger, "%s is accepted", mAcceptorPlaceHolder->toString().c_str());
	client = std::move(mAcceptorPlaceHolder);

	return true;
}

void Iocp::onNewClient(std::unique_ptr<IAcceptor> client)
{
	std::lock_guard<std::mutex> lk(mSync);

	if (mManagedAcceptors.find(client->getId()) != mManagedAcceptors.end())
	{
		LOG_ERROR(mLogger, "duplicate new client %llu", client->getId());
		return;
	}

	DWORD_PTR completionKey = (DWORD_PTR)client.get();
	mIocpHandle = CreateIoCompletionPort((HANDLE)client->getHandle(), mIocpHandle, completionKey, 0);
	if (mIocpHandle == nullptr)
	{
		LOG_FATAL(mLogger, "register completion port failed : %d, err %s", mAcceptorPlaceHolder->getId(), get_last_err_msg());
		return;
	}

	if (!client->read())
	{
		client->close("read failed");
		return;
	}

	LOG_INFO(mLogger, "new client %llu", client->getId());
	mManagedAcceptors.emplace(client->getId(), std::move(client));
}

void Iocp::onLeaveClient(IAcceptor* client)
{
	if (client == nullptr)
	{
		LOG_ERROR(mLogger, "leaving client is null %d");
		return;
	}

	std::lock_guard<std::mutex> lk(mSync);

	auto iter = mManagedAcceptors.find(client->getId());
	if (iter == mManagedAcceptors.end())
	{
		LOG_DEBUG(mLogger, "leaving client no found %d", client->getId());
		return;
	}

	LOG_INFO(mLogger, "leaving client %d", client->getId());
}

DWORD __stdcall Iocp::iocpWorkerThread()
{
	Logger* logger = Logger::getCurrentLogger();
	
	DWORD dwIoSize = 0;
	IAcceptor* client = nullptr;
	CIoData* ioData = nullptr;

	const auto close_client = [](IAcceptor* client, const char* reason)
	{
		client->close(reason);
	};


	while (!mExitFlag)
	{
		bool bSuccess = ::GetQueuedCompletionStatus(mIocpHandle, &dwIoSize, (PULONG_PTR)&client, (LPOVERLAPPED*)&ioData, INFINITE);
		
		if (!bSuccess)
		{
			int errorID = WSAGetLastError();
			if (errorID == WSA_OPERATION_ABORTED || errorID == ERROR_NETNAME_DELETED)
			{
				LOG_INFO(logger, "detect client disconnection");
			}
			else
			{
				LOG_ERROR(logger, "GetQueuedCompletionStatus failed : %s", get_last_err_msg());
			}
		}

		if (mExitFlag)
		{
			LOG_INFO(logger, "exit ...");
			return 0;
		}

		if (ioData == nullptr)
		{
			LOG_INFO(logger, "exit ...");
			return 0;
		}
		
		IO_OPERATION ioType = ioData->getIoType();
		if (client == nullptr)
		{
			LOG_ERROR(logger, "completion key is null ...");
			continue;
		}

		if (!bSuccess || (bSuccess && (dwIoSize == 0)))
		{
			onLeaveClient(client);
			continue;
		}
		
		try 
		{
			if (ioType == IO_OPERATION::READ)
			{
				if(!client->onRead(dwIoSize))
				{
					close_client(client, "onRead failed");
				}

				if (!client->read())
				{
					close_client(client, "read failed");
				}
			}
			else
			{
				if (!client->onWrite(dwIoSize))
				{
					close_client(client, "onWrite failed");
				}
			}
		}
		catch (std::exception e)
		{
			client->close(Format::format("io exception : %s", e.what()).c_str());
#ifdef BOOST_SUPPORT
			std::stringstream ss;
			ss << boost::stacktrace::stacktrace();
			std::string trace = ss.str();
			LOG_ERROR(logger, "Iocp worker exception : %s\n%s", e.what(), trace.c_str());
#else
			LOG_ERROR(logger, "Iocp worker exception : %s", e.what());
#endif
		}
	}

	return 0;
}