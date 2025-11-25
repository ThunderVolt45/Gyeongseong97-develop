#pragma execution_character_set( "utf-8" )

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

#include "GameManager.h"
#include "Utility.h"
#include "AudioManager.h"
#include "ImageLoader.h"

using namespace ftxui;
using namespace std;

// 게임 설정 상수
const std::chrono::milliseconds TICK_TIME = 20ms;

int main()
{
	// 유니코드 및 콘솔 출력 설정 (UTF-8)
	SetConsoleOutputCP(CP_UTF8);

	// 오디오 엔진 초기화
	AudioManager* audioManager = new AudioManager;
	audioManager->PlayAudio(L"sfx.mp3", true);
	audioManager->PlayAudio(L"bgm.mp3", false);

	// 게임 매니저 생성
	GameManager gameManager;

	// 화면 정리
	system("cls");

	// FTXUI 스크린 생성
	auto screen = ScreenInteractive::FitComponent();

	// 컴포넌트 연결
	// Renderer: 화면을 그리는 역할
	auto renderer = Renderer(
		[&]{
			return gameManager.Render();
		}
	);

	// CatchEvent: 키보드 입력을 받아 처리하는 역할
	auto component = CatchEvent(renderer,
		[&](Event event) {
			if (!gameManager.OnEvent(event))
			{
				return false;
			}

			// 게임이 중단되면 종료 신호 전달
			if (!gameManager.IsRunning)
			{
				screen.Exit();
			}

			return true;
		}
	);

	// 게임 로직 스레드 생성
	// UI 렌더링과 별개로 게임의 로직을 일정 간격으로 실행
	thread thread(
		[&] {
			while (gameManager.IsRunning)
			{
				gameManager.Update();

				// 화면 갱신
				screen.Post(Event::Custom);

				// 일정 시간 동안 취침
				this_thread::sleep_for(TICK_TIME);
			}
		}
	);

	// 메인 루프 시작
	screen.Loop(component);

	// 정리
	if (thread.joinable())
	{
		thread.join();
	}

	delete audioManager;

	/*
	// Sprite 로드
	Sprite sprite = ImageLoader::CreateSpriteFromImage(L"image.png", 160, 120);

	if (sprite.sizeX == 0)
	{
		wcerr << L"이미지 파일을 불러오는데 실패했습니다!" << endl;
		delete audioManager;
		return -1;
	}

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

		// Sprite의 모든 픽셀을 순회
		for (int y = 0; y < termHeight; y++)
		{
			if (y > sprite.sizeY) continue;

			for (int x = 0; x < termWidth; x++)
			{
				if (x > sprite.sizeX) continue;

				// Canvas에 색상으로 점(블록)을 찍는다.
				c.DrawBlock(x, y, true, sprite.colors[x + (y * sprite.sizeX)]);
			}
		}

		return canvas(move(c));
		});

	// 화면에 렌더링
	screen.Loop(renderer);
	*/

	return 0;
}