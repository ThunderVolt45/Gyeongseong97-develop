#include "Utility.h"
#include <iostream>
#include <random>
#include <string> // std::string, std::wcslen
#include <vector> // Not strictly necessary for this implementation, but often useful

/*
이게 무슨 개X같은 코드야???

WideCharToMultiByte 같은 플랫폼 종속적인 코드를 제거하기 위해 codecvt 모듈을 쓰려했으나
C++17에서 대체제도 안 만들고 Deprecated 처리한 다음 C++26에서 아예 없애버린댄다.

덕분에 나는 AI 드론봇이 만든, 대체 무슨 원리로 동작하는 건지도 모를
(하지만 동작은 하는 거 같은) 괴코드를 넣어야만 했다.
이 코드의 원리를 이해하는 사람이 있다면 부디 나 대신 주석을 좀 달아주길

왜 windows.h를 제거하려 했냐고? MIM, MAX 매크로랑 자꾸 충돌을 일으켜대서
#define NOMINMAX <- 같은 걸 사방 팔방에 넣어대야 하는 상황이 벌어졌는데
그걸 그냥 손 놓고 구경할 수는 없었거든
*/

// Helper function to convert a single Unicode code point to UTF-8 bytes and append to string
static void append_unicode_codepoint_to_utf8(std::string& out, uint32_t c) {
    if (c < 0x80) { // 1-byte sequence (0-7F)
        out.push_back(static_cast<char>(c));
    } else if (c < 0x800) { // 2-byte sequence (80-7FF)
        out.push_back(static_cast<char>(0xC0 | (c >> 6)));
        out.push_back(static_cast<char>(0x80 | (c & 0x3F)));
    } else if (c < 0x10000) { // 3-byte sequence (800-FFFF)
        out.push_back(static_cast<char>(0xE0 | (c >> 12)));
        out.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (c & 0x3F)));
    } else if (c < 0x110000) { // 4-byte sequence (10000-10FFFF)
        out.push_back(static_cast<char>(0xF0 | (c >> 18)));
        out.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (c & 0x3F)));
    }
    // For invalid Unicode code points (e.g., > 0x10FFFF or within surrogate range but not part of a pair),
    // a more robust implementation might insert U+FFFD (replacement character) or report an error.
    // This implementation assumes valid code points are processed.
}

/// <summary>
/// 대체 무슨 원리로 동작하는 건지도 모를 괴코드의 힘을 빌려
/// wchar_t를 적절하게 utf-8 string으로 변환해주는 메소드
/// </summary>
/// <param name="wideStr">변환하고 싶은 wchar_t</param>
/// <returns>utf-8 문자열</returns>
std::string Utility::ConvertWideToUtf8(const wchar_t* wideStr)
{
    if (wideStr == nullptr || wideStr[0] == L'\0')
    {
        return "";
    }

    std::string utf8str;

    // UTF-8은 가장 긴 경우 4바이트로 하나의 코드 포인트를 표현할 수 있습니다.
    // wchar_t의 길이에 4를 곱하여 충분한 공간을 미리 할당합니다.
    utf8str.reserve(std::wcslen(wideStr) * 4);

    const wchar_t* p = wideStr;
    while (*p != L'\0') {
        uint32_t code_point = *p; // 현재 wide character를 코드 포인트로 가져옵니다.

        // wchar_t가 16비트(대부분의 Windows 시스템)인 경우 서러게이트 페어 처리가 필요합니다.
        // 32비트인 경우(대부분의 Linux 시스템), 이 로직은 건너뛰어집니다.
        if (sizeof(wchar_t) == 2 && (code_point >= 0xD800 && code_point <= 0xDBFF)) { // High surrogate (선행 서러게이트)
            if (*(p + 1) != L'\0' && (*(p + 1) >= 0xDC00 && *(p + 1) <= 0xDFFF)) { // Low surrogate (후행 서러게이트)
                // 서러게이트 페어를 유니코드 코드 포인트로 결합합니다.
                code_point = 0x10000 + ((code_point - 0xD800) << 10) + (*(p + 1) - 0xDC00);
                p++; // 후행 서러게이트도 처리했으므로 포인터를 하나 더 이동합니다.
            } else {
                // 잘못된 서러게이트 페어: 선행 서러게이트 뒤에 후행 서러게이트가 없습니다.
                // 이 경우에는 선행 서러게이트 자체를 하나의 코드 포인트로 간주합니다.
                // 더 엄격한 구현에서는 U+FFFD (대체 문자)를 삽입할 수 있습니다.
            }
        }
        // 후행 서러게이트만 단독으로 나타나는 경우 (잘못된 경우)는 현재 코드에서는
        // code_point >= 0xDC00 && code_point <= 0xDFFF 조건이 없으므로,
        // 해당 문자를 단독 코드 포인트로 처리하게 됩니다.
        // 이는 유효하지 않지만, 입력 문자열이 유효하다고 가정합니다.

        // 완성된 유니코드 코드 포인트를 UTF-8로 변환하여 결과 문자열에 추가합니다.
        append_unicode_codepoint_to_utf8(utf8str, code_point);
        p++; // 다음 wide character로 이동합니다.
    }

    return utf8str;
}

/// <summary>
/// begin과 end 사이의 난수를 생성하는 메서드
/// </summary>
/// <param name="begin">난수 시작점</param>
/// <param name="end">난수 종료점 (난수 범위에 포함)</param>
/// <returns>정수형 난수</returns>
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