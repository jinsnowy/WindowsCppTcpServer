#include "pch.h"
#include "NetworkObjectBase.h"
#include "Logger.h"

std::atomic<unsigned long long> netObjId = 0;

NetworkObjectBase::NetworkObjectBase()
	:
	mLogger(Logger::getInstance()),
	mId(netObjId.fetch_add(1))
{
}