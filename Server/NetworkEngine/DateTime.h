#pragma once
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include "Random.h"

class DateTime : protected tm
{
public:
	static DateTime now()
	{
		auto t = std::time(NULL);
		auto date = DateTime();
		localtime_s((tm*)&date, &t);

		return date;
	}

	static DateTime nowUtc()
	{
		auto t = std::time(NULL);
		auto date = DateTime();
		gmtime_s((tm*)&date, &t);

		return date;
	}

	bool isValid() const
	{
		switch (tm_mon)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			return tm_mday <= 31;
		case 4:
		case 6:
		case 9:
		case 11:
			return tm_mday <= 30;
		case 2:
			return tm_mday <= 28;

		default:
			break;
		}

		return false;
	}

	int getYear() const { return tm_year + 1900; }
	int getDays() const { return tm_mday; }
	int getMonth() const { return tm_mon + 1; }

	std::string toString(const char* format = "%Y-%m-%d %H:%M:%S") const
	{
		std::ostringstream oss;
		oss << std::put_time((tm*)this, format);
		return oss.str();
	}

	std::string toSqlString() const
	{
		return toString("%Y-%m-%d %H:%M:%S");
	}
};

