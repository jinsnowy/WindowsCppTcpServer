#pragma once
class Logger;
class NetworkObjectBase
{
private:
	unsigned long long mId;

protected:
	Logger* mLogger;

protected:
	NetworkObjectBase();
	virtual ~NetworkObjectBase() = default;

public:
	virtual std::string toString() const { return ""; };
	unsigned long long getId() const { return mId; }
};