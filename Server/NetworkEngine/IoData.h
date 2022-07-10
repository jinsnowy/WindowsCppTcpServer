#pragma once
#include <Windows.h>
#include <WinSock2.h>

enum class IO_OPERATION
{
	READ = 0,
	WRITE,
	//ACCEPT
};

struct CIoData
{
	friend class IoDataBuffer;
private:
	WSAOVERLAPPED overlapped;
	WSABUF wsaBuf;
	IO_OPERATION rwMode;
	unsigned long long id;

public:
	CIoData(unsigned long long idIn)
		:
		id(idIn),
		rwMode{}
	{
		ZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
		ZeroMemory(&wsaBuf, sizeof(WSABUF));
	}

	CIoData(unsigned long long idIn, IO_OPERATION rwModeIn)
		:
		id(idIn),
		rwMode(rwModeIn)
	{
		ZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
		ZeroMemory(&wsaBuf, sizeof(WSABUF));
	}

	void setId(unsigned long long idIn) { id = idIn; }

	unsigned long long getId() const { return id; }

	void setBuffer(char* bufferIn, int buflen) { wsaBuf.buf = bufferIn; wsaBuf.len = buflen; }

	char* getBuffer() { return wsaBuf.buf; }

	int getBufferLength() { return wsaBuf.len; }

	IO_OPERATION getIoType() const { return rwMode; }

	LPWSABUF getWsaBuf() { return &wsaBuf; }

	LPWSAOVERLAPPED getOverlapped() { assert(this == (CIoData*)(&overlapped)); return &overlapped; }

	const char* getOpStr() const
	{
		if (rwMode == IO_OPERATION::READ)
			return "READ";
		else if (rwMode == IO_OPERATION::WRITE)
			return "WRITE";
		else
			return "ACCEPT";
	}

};