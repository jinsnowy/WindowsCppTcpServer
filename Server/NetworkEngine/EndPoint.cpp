#include "pch.h"
#include "EndPoint.h"

std::string EndPoint::toString() const
{
	return Format::format("%s:%s", getIpAddress().c_str(), getPort().c_str());
}