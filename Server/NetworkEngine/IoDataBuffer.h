#pragma once
#include "Singleton.h"
#include "IoData.h"
class Logger;
struct CIoDataProxy;

class IoDataBuffer : public ISingleton<IoDataBuffer>
{
	friend class ISingleton<IoDataBuffer>;
private:
	const int REGULAR_BUFFER_SIZE;

	Logger* mLogger;
	std::mutex mSync;
	
	int mCurPos;
	std::vector<CIoData> mCircularBuffer;
	
private:
	IoDataBuffer();

public:
	CIoData* getNext(IO_OPERATION rwMode, char* buf, int len);
};