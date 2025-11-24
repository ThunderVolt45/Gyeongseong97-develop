#pragma execution_character_set( "utf-8" )

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8

#include <stb_image.h>
#include <iostream>
#include <string>

#define NOMINMAX // Prevent min/max macro conflicts with windows.h
#include <windows.h>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/component/component.hpp>

#include "Utility.h"
#include "AudioManager.h"

using namespace std;
using namespace ftxui;

const static string IMAGE_PATH = "\\Sprites\\";

int main()
{
	// 유니코드 설정
	wcout.imbue(locale("kor"));
	wcerr.imbue(locale("kor"));

	// 오디오 엔진 초기화
	AudioManager* audioManager = new AudioManager;
	audioManager->PlayAudio(L"bgm.mp3", false);

	// 이미지 로드
	vector<wchar_t> pathBuffer(MAX_PATH);
	DWORD ret = GetCurrentDirectoryW(MAX_PATH, pathBuffer.data());

	if (ret == 0)
	{
		wcerr << L"Error: 이미지 경로를 가져오는데 실패했습니다." << endl;
		return -1;
	}
	else if (ret == pathBuffer.size())
	{
		wcerr << L"Warning: 이미지 경로가 너무 깁니다. 더 큰 길이 버퍼가 필요합니다." << endl;
	}

	auto strFilePath = Utility::ConvertWideToUtf8(pathBuffer.data()) + IMAGE_PATH + "image.png";
	const char* filePath = strFilePath.c_str();

	int width, height, n_channels;

	// stbi_load 함수로 이미지 파일의 픽셀 데이터를 메모리로 로드합니다.
	// 4번째 인자 '4'는 RGBA 채널(4개)를 강제로 로드하라는 의미입니다.
	unsigned char* data = stbi_load(filePath, &width, &height, &n_channels, 4);

	// 이미지 로드에 실패했다면
	if (data == nullptr)
	{
		wcerr << L"stb: 이미지를 로드할 수 없습니다. " << filePath << L" 파일이 존재하는 지 확인하세요." << endl;
		return 0;
	}

	// 화면 정리
	system("cls");

	// FTXUI 캔버스 생성 및 이미지 그리기
	auto screen = ScreenInteractive::Fullscreen();
	
	auto renderer = Renderer([&] {
		// 터미널의 크기를 가져옴
		//int termWidth = screen.dimx();
		//int termHeight = screen.dimy() * 2; // 캔버스는 반블록을 사용하므로 높이를 2배로 계산

		int termWidth = 160;
		int termHeight = 120;

		// 0으로 나누기 방지
		if (termWidth == 0 || termHeight == 0)
		{
			return text("터미널 크기를 계산하는 중입니다...") | center;
		}

		// 캔버스 크기를 터미널 크기에 맞춤
		auto c = Canvas(termWidth, termHeight);

		// 이미지의 모든 픽셀을 순회
		for (int y = 0; y < termHeight; y++)
		{
			for (int x = 0; x < termWidth; x++)
			{
				// 터미널 좌표(x, y)를 원본 이미지의 좌표(imgX, imgY)로 매핑
				int imgX = x * width / termWidth;
				int imgY = y * height / termHeight;

				// (imgX, imgY) 위치의 픽셀 데이터 시작 인덱스 계산
				unsigned char* pixel_offset = data + (imgX + width * imgY) * 4;

				// RGBA 값 추출
				uint8_t r = pixel_offset[0];
				uint8_t g = pixel_offset[1];
				uint8_t b = pixel_offset[2];
				uint8_t a = pixel_offset[3];

				// 알파 값이 0이면 그리지 않음
				if (a == 0) continue;

				// Canvas에 색상으로 점(블록)을 찍는다.
				c.DrawBlock(x, y, true, Color(r, g, b));
			}
		}

		return canvas(move(c));
		});

	// 화면에 렌더링
	screen.Loop(renderer);

	// 메모리 해제
	stbi_image_free(data);

	delete audioManager;
}