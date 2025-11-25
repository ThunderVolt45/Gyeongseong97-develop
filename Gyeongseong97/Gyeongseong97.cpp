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
	if (thread.joinable()) thread.join();
	delete audioManager;

	return 0;
}