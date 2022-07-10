#pragma once

#include <iostream>
#include <windows.h>

class critical_section
{
private:
	CRITICAL_SECTION mHandle;

public:
	critical_section()
	{
#pragma warning(disable:6031)
		InitializeCriticalSectionAndSpinCount(&mHandle, 100);
#pragma warning(default:6031)
	}

	~critical_section()
	{
		DeleteCriticalSection(&mHandle);
	}

	void lock()
	{
		EnterCriticalSection(&mHandle);
	}

	void unlock()
	{
		LeaveCriticalSection(&mHandle);
	}
};

class spin_lock
{
private:
	volatile long mLock;

public:
	spin_lock()
		:
		mLock(0)
	{}

	void lock()
	{
		while (InterlockedCompareExchange(&mLock, 1, 0) == 1);
	}

	void unlock()
	{
		if (InterlockedCompareExchange(&mLock, 0, 1) == 0)
		{
			throw std::runtime_error("SpinLock was not locked");
		}
	}
};