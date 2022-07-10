#pragma once
#include <iostream>
#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

class Format
{
private:
	static std::string empty;

public:
	template<typename ...Args>
	static std::string format(const char* fmt, Args&& ...args)
	{
		int size = std::snprintf(nullptr, 0, fmt, std::forward<Args>(args)...) + 1;
		if (size <= 1)
			return std::string();

		std::string s(size, '\0');
		std::snprintf(&s[0], size, fmt, std::forward<Args>(args)...);
		s.pop_back();

		return s;
	}

	template<unsigned int BUF_SIZE = 512, typename ...Args>
	static const char* format_s(const char* fmt, Args&& ...args)
	{
		int size = std::snprintf(nullptr, 0, fmt, std::forward<Args>(args)...) + 1;
		if (size <= 1)
			return std::string();

		thread_local char buffer[BUF_SIZE];
		memset(buffer, 0, BUF_SIZE);
		std::snprintf(buffer, size, fmt, std::forward<Args>(args)...);

		return buffer;
	}

	static std::wstring toWide(const std::string& str)
	{
		if (str.size() >= 512)
			return L"overflows";

		size_t szNumberOfConverted = str.size();
		wchar_t buffer[512];
		mbstowcs_s(&szNumberOfConverted, buffer, str.c_str(), 512);

		return buffer;
	}

	static std::string toNarrow(const std::wstring& str)
	{
		if (str.size() >= 512)
			return "overflows";

		CHAR buffer[512];
		::WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, buffer, 512, 0, 0);

		return buffer;
	}

	template<typename Iter>
	static std::string join(Iter first, Iter last, const std::string& delimiter)
	{
		auto iter = first;
		std::stringstream ss;
		for (; iter != last;)
		{
			ss << *iter; ++iter;
			if (iter != last)
				ss << delimiter;
		}

		return ss.str();
	}

	static std::vector<std::string> split(std::string s, const std::string& delimiter)
	{
		std::vector<std::string> result;

		size_t pos = 0;
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos) 
		{
			result.push_back(s.substr(0, pos));
			s.erase(0, pos + delimiter.length());
		}

		if (!s.empty())
			result.push_back(std::move(s));

		return result;
	}

};