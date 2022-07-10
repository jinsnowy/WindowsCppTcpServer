#include "pch.h"
#include "Logger.h"
#include "PathManager.h"

Logger::Logger()
	:
	mExitFlag(false),
	mConsoleLog(true),
	mFlushDurationMilliSec(1000),
	mWorker([this]() { this->flush(); }),
	mLogLevel(ELogLevel::Info)
{
	char buffer[MAX_PATH];
	::GetModuleFileName(NULL, buffer, MAX_PATH);
	basePath = buffer;

	size_t pos = basePath.find_last_of('\\');
	basePath = basePath.substr(0, pos);
}

Logger::~Logger()
{
	{
		std::lock_guard<std::mutex> lk(mSync);
		mExitFlag = true;
		mCV.notify_one();
	}

	if (mWorker.joinable())
	{
		mWorker.join();
	}

	if (mOutFile.is_open())
	{
		mOutFile.close();
	}
}

void Logger::write(const std::string& logs)
{
	try 
	{
		if (mOutFile.is_open() == false)
		{
			DateTime now = DateTime::now();
			std::string log_file_name = Format::format("%s%d.%d.%d.log", programName.c_str(), now.getYear(), now.getMonth(), now.getDays());
			std::string log_file_path = Format::format("%s\\%s", basePath.c_str(), log_file_name.c_str());

			mOutFile.open(log_file_path, std::ios_base::out | std::ios_base::app);
		}

		if (mOutFile.is_open() == false)
		{
			return;
		}

		mOutFile << logs;
		mOutFile.flush();

		if (mConsoleLog)
		{
			std::cout << logs;
		}
	}
	catch (std::exception e)
	{
		std::cerr << "log write failed : " << e.what() << std::endl;
	}
}

void Logger::flush()
{
	std::string logs;
	const auto duration = std::chrono::milliseconds(mFlushDurationMilliSec);
	while (!mExitFlag)
	{
		std::unique_lock<std::mutex> lk(mSync);

		mCV.wait_for(lk, duration, [this, &logs](){ return mExitFlag; });

		if (logs.empty())
		{
			logs = mBuffer.str();
		}

		mBuffer.str("");

		lk.unlock();
	
		if (!logs.empty())
		{
			write(logs);
			logs.clear();
		}
	}
}