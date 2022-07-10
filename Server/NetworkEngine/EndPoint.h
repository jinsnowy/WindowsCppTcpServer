#pragma once

class EndPoint
{
private:
    SOCKADDR_IN mInfo;

public:
	EndPoint()
		:
		mInfo{}
	{
	}

    EndPoint(const SOCKADDR_IN& info)
        :
        mInfo(info)
    {
    }

	EndPoint(uint16 port)
	{
		mInfo.sin_family = AF_INET;
		mInfo.sin_addr.s_addr = htonl(INADDR_ANY);
		mInfo.sin_port = htons(port);
	}

    EndPoint(const char* ipAddress, uint16 port)
    {
        mInfo.sin_family = AF_INET;
        mInfo.sin_addr.s_addr = inet_addr(ipAddress);
        mInfo.sin_port = htons(port);
    }

	std::string getIpAddress() const
	{
		return inet_ntoa(mInfo.sin_addr);
	}

	std::string getPort() const
	{
		return std::to_string(ntohs(mInfo.sin_port));
	}

	const SOCKADDR* getData() const
	{
		return (SOCKADDR*)(&mInfo);
	}
	
	constexpr size_t getSize() const { return sizeof(SOCKADDR); }

	std::string toString() const;

	static bool resolve(const char* hostAddress, uint16 port, EndPoint& endPoint)
	{
		addrinfo* result = nullptr;
		addrinfo hints = {};
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		auto err_code = getaddrinfo(hostAddress, std::to_string(port).c_str(), &hints, &result);
		if (err_code != 0 || result == nullptr)
		{
			return false;
		}

		SOCKADDR_IN* address = (SOCKADDR_IN*)(result->ai_addr);
		endPoint = EndPoint(*address);
		freeaddrinfo(result);

		return true;
	}
};