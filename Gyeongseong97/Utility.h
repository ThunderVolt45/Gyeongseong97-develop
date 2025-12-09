#pragma once
#include <string>
#include <vector>

class Utility
{
public:
	/// <summary>
	/// 대체 무슨 원리로 동작하는 건지도 모를 괴코드의 힘을 빌려
	/// wchar_t를 적절하게 utf-8 string으로 변환해주는 메소드
	/// </summary>
	/// <param name="wideStr">변환하고 싶은 wchar_t</param>
	/// <returns>utf-8 문자열</returns>
	static std::string ConvertWideToUtf8(const wchar_t* wideStr);

	/// <summary>
	/// begin과 end 사이의 무작위 정수 값을 반환하는 유틸리티 함수
	/// </summary>
	/// <param name="begin">시작 값</param>
	/// <param name="end">끝 값</param>
	/// <returns>무작위 정수</returns>
	static int GenerateRandomNumber(int begin, int end);

	/// <summary>
	/// 텍스트 파일을 읽어들여 vector에 한 줄씩 집어넣는 유틸리티 함수vector<string>
	/// </summary>
	/// <param name="filePath">읽어들일 파일의 경로</param>
	/// <param name="targetVector"></param>
	static void ReadTextFile(std::wstring fileName, std::vector<std::string>& targetVector);
};