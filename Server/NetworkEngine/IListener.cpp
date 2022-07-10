#include "pch.h"
#include "IListener.h"

IListener::~IListener()
{
}

bool IListener::listen(const uint16& port, int backlog)
{
	return false;
}

bool IListener::accept(SOCKET hAcceptSocket)
{
	return false;
}
