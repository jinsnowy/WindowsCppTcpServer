#pragma once
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "Singleton.h"

enum class ELogLevel
{
	Info,
	Debug,
	Warn,
	Error,
	Fatal,
};

struct LogLevelStruct
{
	static const char* GetTag(ELogLevel level)
	{
		switch (level)
		{
		case ELogLevel::Info:
			return "Info";
		case ELogLevel::Debug:
			return "Debug";
		case ELogLevel::Warn:
			return "Warn";
		case ELogLevel::Error:
			return "Error";
		case ELogLevel::Fatal:
			return "Fatal";
		default:
			return "Unknown";
		}
	}
};

class Logger : public ISingleton<Logger>
{
	friend class ISingleton<Logger>;
private:
	bool mExitFlag;
	bool mConsoleLog;
	int mFlushDurationMilliSec;
	std::string basePath;
	std::string programName;

	std::ofstream mOutFile;
	std::stringstream mBuffer;
	
	std::condition_variable mCV;
	std::thread mWorker;
	std::mutex  mSync;

	ELogLevel mLogLevel;
private:
	Logger();

public:
	~Logger();

	std::chrono::steady_clock::time_point st;

	static Logger* getCurrentLogger() { return getInstance(); }

	void setConsoleLog(bool bConsoleLog) { mConsoleLog = bConsoleLog; }

	void setLogLevel(ELogLevel eLoglevel) { mLogLevel = eLoglevel; }

	void setProgramName(const char* name) { programName = name; }

	template<typename ...Args>
	void out(ELogLevel level, std::thread::id thread_id, int line, const char* function, const char* fmt, Args&&... args)
	{
		if (level < mLogLevel)
			return;

		DateTime now = DateTime::now();
		std::stringstream ss;
		ss << thread_id;

		std::string log_str = Format::format(fmt, std::forward<Args>(args)...);
		std::string message = Format::format("[%s] [%s] %s [%d] : %s\n", now.toString().c_str(), ss.str().c_str(), function, line, log_str.c_str());
		
		{
			std::lock_guard<std::mutex> lk(mSync);
			mBuffer << message;
		}
	}
private:
	void write(const std::string& logs);

	void flush();
};

#define LOG_FATAL(logger, fmt, ...) logger->out(ELogLevel::Fatal, std::this_thread::get_id(), __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOG_ERROR(logger, fmt, ...) logger->out(ELogLevel::Error, std::this_thread::get_id(), __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOG_WARN(logger, fmt, ...) logger->out(ELogLevel::Warn, std::this_thread::get_id(), __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOG_DEBUG(logger, fmt, ...) logger->out(ELogLevel::Debug, std::this_thread::get_id(), __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOG_INFO(logger, fmt, ...) logger->out(ELogLevel::Info, std::this_thread::get_id(), __LINE__, __FUNCTION__, fmt, __VA_ARGS__)

#endif