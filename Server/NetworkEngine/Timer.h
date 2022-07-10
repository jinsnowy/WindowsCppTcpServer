#pragma once
#include <chrono>
#include <ctime>

class Timer
{
	using Clock = std::chrono::steady_clock;
	using TimePoint = std::chrono::steady_clock::time_point;
private:
	TimePoint mStart;
	Timer() : mStart(Clock::now()) {}

public:
	static Timer startNew() noexcept { return Timer(); }

	template<typename T>
	auto count()
	{
		return std::chrono::duration_cast<T>(Clock::now() - mStart).count();
	}

	template<typename T>
	static auto count(const TimePoint& start)
	{
		return std::chrono::duration_cast<T>(Clock::now() - start).count();
	}
};