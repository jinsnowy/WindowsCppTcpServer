#include "pch.h"
#include "IoDataBuffer.h"
#include "Logger.h"
#include "NetworkEngine.h"

static std::atomic<unsigned long long> idDataBufGen = 0;

IoDataBuffer::IoDataBuffer()
    :
    mCurPos(0),
    mLogger(Logger::getCurrentLogger()),
    REGULAR_BUFFER_SIZE(128)
{
    const_cast<int&>(REGULAR_BUFFER_SIZE) = std::max(NetworkEngine::getInstance()->getIoDataBufferNum(), REGULAR_BUFFER_SIZE);

    for (int i = 0; i < REGULAR_BUFFER_SIZE; ++i)
    {
        mCircularBuffer.emplace_back(idDataBufGen.fetch_add(1));
    }
}

CIoData* IoDataBuffer::getNext(IO_OPERATION rwMode, char* buf, int len)
{
    std::lock_guard<std::mutex> lk(mSync);

    CIoData* ioData = nullptr;
    int next = mCurPos;
    mCurPos = (mCurPos + 1) % REGULAR_BUFFER_SIZE;

    ioData = &mCircularBuffer[next];
    memset(&ioData->overlapped, 0, sizeof(OVERLAPPED));
    ioData->wsaBuf.buf = buf;
    ioData->wsaBuf.len = len;
    ioData->rwMode = rwMode;

    return ioData;
}