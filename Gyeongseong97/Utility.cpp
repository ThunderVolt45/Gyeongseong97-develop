#define _HAS_STD_BYTE 0

#include "Utility.h"
#include <iostream>
#include <random>
#include <windows.h>

std::string Utility::ConvertWideToUtf8(const wchar_t* wideStr)
{
	if (wideStr == nullptr || wideStr[0] == L'\0')
	{
		return "";
	}

	int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);

	if (requiredSize == 0)
	{
		std::wcerr << L"Error: UTF-8 변환을 위해 필요한 버퍼 크기를 확보하는데 실패했습니다. Error code: " << GetLastError() << std::endl;
		return "";
	}

	std::string utf8str(requiredSize, 0);
	int result = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, &utf8str[0], requiredSize, NULL, NULL);

	if (result == 0)
	{
		std::wcerr << L"Error: wide string를 UTF-8로 변환하는데 실패했습니다. Error code: " << GetLastError() << std::endl;
		return "";
	}

	utf8str.resize(requiredSize - 1);

	return utf8str;
}


int Utility::GenerateRandomNumber(int begin, int end)
{
	// 난수 생성기 초기화
	std::random_device random;
	std::mt19937 engine(random());

	// 난수 범위 설정
	std::uniform_int_distribution<int> distribution(begin, end);

	// 난수 생성
	return distribution(engine);
}