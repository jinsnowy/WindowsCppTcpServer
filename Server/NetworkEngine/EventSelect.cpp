#include "pch.h"
#include "EventSelect.h"
#include "TcpSocket.h"

EventSelect::EventSelect()
    :
	mLogger(Logger::getInstance()),
	mThreadHandler(nullptr),
	mDisposed(false)
{
}

EventSelect::~EventSelect()
{
}

bool EventSelect::initialize()
{
	if (mDisposed)
	{
		LOG_ERROR(mLogger, "already disposed");
		return false;
	}

	mThreadHandler.reset(new EventSelect::SelectThreadHandler(*this));

	return mThreadHandler->initialize();
}

void EventSelect::shutdown()
{
    bool expected = false;
	if (mDisposed.compare_exchange_strong(expected, true) == false)
	{
		LOG_ERROR(mLogger, "already disposed");
		return;
	}

	if (mThreadHandler)
	{
		mThreadHandler->shutdown();
		mThreadHandler = nullptr;
	}
}

EventSelect::SelectThreadHandler::SelectThreadHandler(EventSelect& OwnerIn)
    :
	mOwner(OwnerIn),
    mSelectEvent(NULL),
    mStartupEvent(NULL),
    mDestroyEvent(NULL),
	mDisposed(false),
    mLogger(Logger::getInstance())
{
}

EventSelect::SelectThreadHandler::~SelectThreadHandler()
{
    if (!mDisposed)
    {
        shutdown();
    }
}

bool EventSelect::SelectThreadHandler::initialize()
{
	if (mDisposed)
	{
		LOG_ERROR(mLogger, "select thread handler disposed");
		return false;
	}

    safe_close_event(mSelectEvent);
    safe_close_event(mDestroyEvent);
    safe_close_event(mStartupEvent);

	mSelectEvent = WSACreateEvent();
	mDestroyEvent = WSACreateEvent();
	mStartupEvent = WSACreateEvent();

	mSelectThread = std::thread([this] { SelectThreadCallback(); });

	DWORD dwResult =::WaitForSingleObject(mStartupEvent, 10000);
	if (dwResult != WAIT_OBJECT_0)
	{
		LOG_ERROR(mLogger, "init select thread error %s", get_last_err_msg());
		return false;
	}

	SOCKET hSocket = mOwner.getSocketHandle();
	if (WSAEventSelect(hSocket, mSelectEvent, FD_CONNECT | FD_CLOSE | FD_READ) != 0)
	{
		LOG_ERROR(mLogger, "WSAEventSelect failed : %s", get_last_err_msg());
		return false;
	}

    return true;
}

void EventSelect::SelectThreadHandler::shutdown()
{
    bool expected = false;
    if (mDisposed.compare_exchange_strong(expected, true) == false)
    {
        LOG_ERROR(mLogger, "select thread handler already shutdown");
        return;
    }

	if (mSelectThread.joinable())
	{
		SetEvent(mDestroyEvent);
		mSelectThread.join();
	}

    safe_close_event(mSelectEvent);
    safe_close_event(mDestroyEvent);
    safe_close_event(mStartupEvent);
}

void EventSelect::SelectThreadHandler::SelectThreadCallback()
{
	WSANETWORKEVENTS networkEvents = {};
	DWORD eventID = 0;
	DWORD result = 0;

	HANDLE ThreadEvents[2] = { mDestroyEvent, mSelectEvent };

	const DWORD destroyEventID = WAIT_OBJECT_0;
	const DWORD normalEventID = WAIT_OBJECT_0 + 1;
	SOCKET hSocket = mOwner.getSocketHandle();

	SetEvent(mStartupEvent);

	while (true)
	{
		eventID = ::WaitForMultipleObjects(2, ThreadEvents, false, INFINITE);

		switch (eventID)
		{
			case destroyEventID:
			{
				LOG_INFO(mLogger, "destroy event on select thread");
				return;
			}
			case normalEventID:
			{
				eventID = ::WSAEnumNetworkEvents(hSocket, mSelectEvent, &networkEvents);

				if (eventID == SOCKET_ERROR)
				{
					LOG_ERROR(mLogger, "socket error in event select : %s", get_last_err_msg());
					return;
				}
				else
				{
					if (networkEvents.lNetworkEvents & FD_CONNECT)
					{
						int errCode = networkEvents.iErrorCode[FD_CONNECT_BIT];
						if (errCode)
						{
							LOG_ERROR(mLogger, "connect error in event select : %d %s", errCode, get_last_err_msg_code(errCode));

							mOwner.onIoConnectedFailed();
							continue;
						}

						mOwner.onIoConnectedSucessfully();
					}
					else if (networkEvents.lNetworkEvents & FD_READ)
					{
						int errCode = networkEvents.iErrorCode[FD_READ_BIT];
						if (errCode)
						{
							LOG_ERROR(mLogger, "read error in event select : %d %s", errCode, get_last_err_msg_code(errCode));
							continue;
						}

						mOwner.onIoRead();
					}
					else if (networkEvents.lNetworkEvents & FD_CLOSE)
					{
						int errCode = networkEvents.iErrorCode[FD_CLOSE_BIT];
						if (errCode)
						{
							if (errCode == WSAECONNABORTED)
							{
								LOG_ERROR(mLogger, "server program shutdown");
							}
							else
							{
								LOG_ERROR(mLogger, "close error in event select : %d %s", errCode, get_last_err_msg_code(errCode));
							}
						
							continue;
						}

						mOwner.onIoDisconnected();
					}
				}
			}break;
			default:
				LOG_ERROR(mLogger, "unknown event in event select : %s", get_last_err_msg());
				return;
		}
	}
}
