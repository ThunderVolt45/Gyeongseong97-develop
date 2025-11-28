#pragma once
#include <string>

class Utility
{
public:
	static std::string ConvertWideToUtf8(const wchar_t* wideStr);

	/// <summary>
	/// begin과 end 사이의 무작위 정수 값을 반환하는 유틸리티 함수
	/// </summary>
	/// <param name="begin">시작 값</param>
	/// <param name="end">끝 값</param>
	/// <returns>무작위 정수</returns>
	static int GenerateRandomNumber(int begin, int end);
};