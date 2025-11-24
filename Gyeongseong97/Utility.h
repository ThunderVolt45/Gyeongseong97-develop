#pragma once
#include <string>

class Utility
{
public:
	static std::string ConvertWideToUtf8(const wchar_t* wideStr);
};