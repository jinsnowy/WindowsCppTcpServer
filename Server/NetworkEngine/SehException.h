#pragma once
#include "Format.h"
#include <stdio.h>
#include <windows.h>
#include <eh.h>
#include <exception>

class SE_Exception : public std::exception
{
private:
	const unsigned int nSE;

public:
	SE_Exception() noexcept : SE_Exception{ 0 } {}
	SE_Exception(unsigned int n) noexcept : nSE{ n }, std::exception(getMessage(n)) {}
	unsigned int getSeNumber() const noexcept { return nSE; }

private:
	const char* getMessage(unsigned int code)
	{
		static char buffer[128];
		memset(buffer, 0, std::size(buffer));
		snprintf(buffer, std::size(buffer), "SEHException Code : (%d)", code);

		return buffer;
	}
};

class Scoped_SE_Translator
{
private:
	const _se_translator_function old_SE_translator;

public:
	Scoped_SE_Translator(_se_translator_function new_SE_translator) noexcept
		: old_SE_translator{ _set_se_translator(new_SE_translator) } {}
	~Scoped_SE_Translator() noexcept { _set_se_translator(old_SE_translator); }

	Scoped_SE_Translator()
		:
		Scoped_SE_Translator(trans_func)
	{}

private:
	static void trans_func(unsigned int u, EXCEPTION_POINTERS*)
	{
		throw SE_Exception(u);
	}
};