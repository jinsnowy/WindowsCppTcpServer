#pragma once

class IConnector;
class Logger;
class EventSelect
{
	class SelectThreadHandler;
public:
	EventSelect();
	virtual ~EventSelect();

public:
	virtual bool initialize();

	virtual void shutdown();

protected:
	virtual void onIoRead() =0 ;
	virtual void onIoConnectedSucessfully() = 0;
	virtual void onIoConnectedFailed() = 0;
	virtual void onIoDisconnected() = 0;
	virtual SOCKET getSocketHandle() = 0;

private:
	std::atomic<bool> mDisposed;
	std::unique_ptr<SelectThreadHandler> mThreadHandler;

protected:
	Logger* mLogger;

private:
	class SelectThreadHandler
	{
	private:
		HANDLE mSelectEvent;
		HANDLE mDestroyEvent;
		HANDLE mStartupEvent;

		std::atomic<bool> mDisposed;
		EventSelect& mOwner;
		Logger* mLogger;
		std::thread  mSelectThread;
	public:
		SelectThreadHandler(EventSelect& OwnerIn);
		~SelectThreadHandler();

	public:
		bool initialize();

		void shutdown();

	private:
		void SelectThreadCallback();
	};
};

