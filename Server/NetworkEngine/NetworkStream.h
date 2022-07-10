#pragma once

#include "Serializer.h"
#include "Logger.h"
#include "NetworkEngine.h"

class ReadStream
{
private:
	static constexpr int BUF_SIZE = 4096;
	
private:
	int mDataPos;
	int mCurPos;
	unsigned char mBuffer[BUF_SIZE];
	Logger* mLogger;

public:
	ReadStream()
		:
		mCurPos(0), mDataPos(0), mLogger(Logger::getInstance())
	{
		memset(mBuffer, 0, sizeof(mBuffer));
	}

	void rotate() 
	{
		const int dataLength = mCurPos - mDataPos;
		memmove(mBuffer, mBuffer + mDataPos, dataLength);
		memset(mBuffer + dataLength, 0, BUF_SIZE - dataLength);
		mDataPos = 0;
		mCurPos = dataLength;
	}

	int available() const { return BUF_SIZE - mCurPos; }
	
	bool isAvailable(int nBytes) const { return mCurPos + nBytes < BUF_SIZE; }

	char* getCurPtr() { return (char*)mBuffer + mCurPos; }
	
	int getCurPos() const { return mCurPos; }

	constexpr int getBufSize() const { return BUF_SIZE; }

	bool checkRecvBytes(int recvBytes)
	{
		if (mCurPos + recvBytes >= BUF_SIZE)
			return false;

		int pktLength = Serializer::toInt32(mBuffer + mDataPos);
		if (pktLength <= 0)
		{
			LOG_ERROR(mLogger, "invalid pktlength(%d) on recv(%d) (last err = %s)", pktLength, recvBytes, get_last_err_msg());
			return false;
		}
			
		mCurPos += recvBytes;

		return true;
	}

	bool readCodeBytes(int& code, int& pktSize, std::vector<unsigned char>& byteStream)
	{
		// fully dispatched
		if (mCurPos == mDataPos)
			return false;

		int dataLength = mCurPos - mDataPos;
		int pktLength = Serializer::toInt32(mBuffer + mDataPos);

		// not received full packet
		if (dataLength < pktLength)
			return false;

		pktSize = pktLength;
		code = Serializer::toInt32(mBuffer + mDataPos + sizeof(int));
		byteStream.resize(pktLength);
		memcpy(byteStream.data(), mBuffer + mDataPos, pktLength);
		mDataPos += pktLength;

		return true;
	}

	std::string checkRecvBytesErrMsg(int recvBytes) const
	{
		if (mCurPos + recvBytes >= BUF_SIZE)
			return Format::format("sum(%d) = cur(%d) + recv(%d) > buffer size(%d)", mCurPos + recvBytes, mCurPos, recvBytes, BUF_SIZE);

		//int pktLength = Serializer::toInt32((unsigned char*)mBuffer + mCurPos);
		//int dataLength = mCurPos - mDataPos + recvBytes;
		//if (pktLength > dataLength)
		//	return Format::format("pktLength(%d) > dataLength(%d)", pktLength, dataLength);

		return "None";
	}

	std::string getVerbose() const
	{
		char buffer[64];
		memset(buffer, 0, 64);
		sprintf_s(buffer, "CurPos(%d) DataPos(%d)", mCurPos, mDataPos);
		return buffer;
	}
};

class WriteStream
{
public:
	struct StaticPageSizeInfo
	{
		int pageSize;
		StaticPageSizeInfo()
		{
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			pageSize = sysInfo.dwPageSize;
		}
	};
private:
	int mCurPos;  // cur pending pos;
	int mCurBlock; // cur peding block

	const int BUF_MAX_ARR_SIZE;
	const int BUFFER_SIZE;
	std::vector<std::unique_ptr<unsigned char[]>> mBuffer;
	std::mutex mSync;
public:
	WriteStream()
		:
		mCurPos(0),
		mCurBlock(0),
		BUF_MAX_ARR_SIZE(1),
		BUFFER_SIZE(1024)
	{
		static StaticPageSizeInfo sysInfo;
		const_cast<int&>(BUFFER_SIZE) = std::max(sysInfo.pageSize, BUFFER_SIZE);
		const_cast<int&>(BUF_MAX_ARR_SIZE) = std::max(NetworkEngine::getInstance()->getWriteBufferBlockNum(), BUF_MAX_ARR_SIZE);

		mBuffer.resize(BUF_MAX_ARR_SIZE);
	}

	bool getWriteBuffer(int nBytes, unsigned char** bufferPtr)
	{
		if (bufferPtr == nullptr)
		{
			return false;
		}
		
		std::lock_guard<std::mutex> lk(mSync);

		if (mCurPos + nBytes > BUFFER_SIZE)
		{
			mCurBlock = (mCurBlock + 1) % BUF_MAX_ARR_SIZE;
			mBuffer[mCurBlock].reset(new unsigned char[BUFFER_SIZE]);
			memset(mBuffer[mCurBlock].get(), 0, BUFFER_SIZE);
			mCurPos = 0;
		}

		if (mBuffer[mCurBlock] == nullptr)
		{
			mBuffer[mCurBlock].reset(new unsigned char[BUFFER_SIZE]);
			memset(mBuffer[mCurBlock].get(), 0, BUFFER_SIZE);
		}

		*bufferPtr = mBuffer[mCurBlock].get() + mCurPos;
		mCurPos += nBytes;

		return true;
	}

	int getCurPos() const { return mCurPos; }

	int getCurBlock() const { return mCurBlock; }

	std::string getVerbose() const
	{
		char buffer[64];
		memset(buffer, 0, 64);
		sprintf_s(buffer, "CurPos(%d) CurBlock(%d)", mCurPos, mCurBlock);
		return buffer;
	}
};