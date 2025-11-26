#pragma execution_character_set( "utf-8" )

#include <stb_image.h>
#include <iostream>
#include <string>

#define NOMINMAX // Prevent min/max macro conflicts with windows.h
#include <windows.h>
#include <stdlib.h>

#include <ftxui/component/captured_mouse.hpp>
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

#pragma region Declarations
void TitleScreen(ScreenInteractive& screen);
void IntroScreen(ScreenInteractive& screen);
void GameLoop(ScreenInteractive& screen);
#pragma endregion

// 게임 설정 상수
const std::chrono::milliseconds TICK_TIME = 20ms;

int main()
{
	// 유니코드 및 콘솔 출력 설정 (UTF-8)
	SetConsoleOutputCP(CP_UTF8);

	// 오디오 엔진 초기화
	AudioManager* audioManager = new AudioManager;
	audioManager->PlayAudio(L"sfx.mp3", true);
	audioManager->PlayAudio(L"bgm.mp3", true);

	// 화면 정리
	system("cls");

	// FTXUI Screen 생성
	auto screen = ScreenInteractive::FitComponent();

	// 타이틀 화면 출력
	TitleScreen(screen);

	// 인트로 화면 출력
	IntroScreen(screen);

	// 게임 시작
	GameLoop(screen);

	// 정리
	delete audioManager;

	return 0;
}

void TitleScreen(ScreenInteractive& screen)
{
	// 버튼
	auto button = Container::Horizontal({
		Button(
			"  게임 시작  ", 
			[&] { 
				screen.Exit(); 
				return; 
			}, 
			ButtonOption::Animated(Color::Red)
		) | center,
		Button(
			" 크레딧 ",
			[&] {
				return;
			},
			ButtonOption::Animated(Color::Green)
		) | center,
		Button(
			" 나가기 ",
			[&] {
				exit(0); // 즉시 프로그램을 폭☆8 시킨다
				return;
			},
			ButtonOption::Animated(Color::Yellow)
		) | center
	});

	// 타이틀 화면 출력
	auto titleScreenRenderer = Renderer(button, 
		[&] {
			return vbox({
				/*

				_____                                                                     _____  ______
				|  __ \                                                                   |  _  ||___  /
				| |  \/ _   _   ___   ___   _ __    __ _  ___   ___   ___   _ __    __ _  | |_| |   / /
				| | __ | | | | / _ \ / _ \ | '_ \  / _` |/ __| / _ \ / _ \ | '_ \  / _` | \____ |  / /
				| |_\ \| |_| ||  __/| (_) || | | || (_| |\__ \|  __/| (_) || | | || (_| | .___/ /./ /
				 \____/ \__, | \___| \___/ |_| |_| \__, ||___/ \___| \___/ |_| |_| \__, | \____/ \_/
						 __/ |                      __/ |                           __/ |
						|___/                      |___/                           |___/

				*/
				text(L""),
				text(L""),
				text(L"    _____                                                                     _____  ______   ") | bold | color(Color::Red) | center,
				text(L"   |  __ \\                                                                   |  _  ||___  /   ") | bold | color(Color::Red) | center,
				text(L"   | |  \\/ _   _   ___   ___   _ __    __ _  ___   ___   ___   _ __    __ _  | |_| |   / /    ") | bold | color(Color::Red) | center,
				text(L"   | | __ | | | | / _ \\ / _ \\ | '_ \\  / _` |/ __| / _ \\ / _ \\ | '_ \\  / _` | \\____ |  / /     ") | bold | color(Color::Red) | center,
				text(L"   | |_\\ \\| |_| ||  __/| (_) || | | || (_| |\\__ \\|  __/| (_) || | | || (_| | .___/ /./ /      ") | bold | color(Color::Red) | center,
				text(L"    \\____/ \\__, | \\___| \\___/ |_| |_| \\__, ||___/ \\___| \\___/ |_| |_| \\__, | \\____/ \\_/       ") | bold | color(Color::Red) | center,
				text(L"            __/ |                      __/ |                           __/ |                  ") | bold | color(Color::Red) | center,
				text(L"           |___/                      |___/                           |___/                   ") | bold | color(Color::Red) | center,
				text(L""),
				text(L""),
				text(L"홍콩 97 / 야인시대 패러디 게임") | color(Color::White) | center,
				text(L"(이 게임은 제작자의 정치적 성향과는 어떠한 연관도 없으며 그냥 웃기려고 만든 겁니다)") | color(Color::White) | center,
				text(L""),
				button->Render() | center
			}) | border | center | color(Color::Red);
		}
	);

	screen.Loop(titleScreenRenderer);

	// 화면 정리
	system("cls");
}

void IntroScreen(ScreenInteractive& screen)
{

}

void GameLoop(ScreenInteractive& screen)
{
	// 게임 매니저 생성
	GameManager gameManager;

	// 컴포넌트 연결
	// Renderer: 화면을 그리는 역할
	auto renderer = Renderer(
		[&] {
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
	std::thread thread(
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
}
