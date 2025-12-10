#include "GameApp.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>

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
#include "RenderSystem.h"
#include "GameConstants.h"
#include "Utility.h"
#include "AudioManager.h"
#include "ImageLoader.h"

using namespace ftxui;
using namespace std;

GameApp::GameApp() : currentAppState(TITLE_SCREEN)
{
}

GameApp::~GameApp()
{
}

void GameApp::Run()
{
	// 오디오 엔진 초기화
	AudioManager& audioManager = AudioManager::GetInstance();
	audioManager.PlayAudio(BGM_MAIN.data(), BGM_VOULME, true);

	// 화면 정리
	ClearScreen();

	// FTXUI Screen 생성
	auto screen = ScreenInteractive::FitComponent();

	// 프로그램 루프 시작
	while (currentAppState != EXIT)
	{
		switch (currentAppState)
		{
			case TITLE_SCREEN: // 타이틀
				currentAppState = TitleScreen(screen);
				break;
			case INTRO_CUTSCENE: // 컷신
				currentAppState = DrawCutscene(screen,
					L"intro_sy.png",
					L"",
					L"1946년이 다가왔다.",
					L"X같이 못생긴 심영이 학생과 시민들을 빨갱이로 만들기 시작했다!",
					L"",
                    INTRO_CUTSCENE // Next state if user exits this cutscene
				);
				if (currentAppState != INTRO_CUTSCENE) break;

				currentAppState = DrawCutscene(screen,
					L"intro_reds.png",
					L"",
					L"경성이 빨갱이로 가득차버렸다! 경성은 엉망이 되었다!",
					L"그리하여 비밀 결사 백의사에서는 백색 테러리스트 김두한을 불렀다.",
					L"",
                    INTRO_CUTSCENE // Next state if user exits this cutscene
				);
				if (currentAppState != INTRO_CUTSCENE) break;

				currentAppState = DrawCutscene(screen,
					L"intro_kdh.png",
					L"",
					L"김두한은 조선의 주먹 황제이자 백색 테러주의자이다.",
					L"1972억 공산당 빨갱이들을 모조리 시체로 결☆ 정 시킬 것이다!",
					L"",
                    INTRO_CUTSCENE // Next state if user exits this cutscene
				);
				if (currentAppState != INTRO_CUTSCENE) break;

				currentAppState = DrawCutscene(screen,
					L"intro_narration.png",
					L"",
					L"그러나 공산당에서는 이미 비밀 연구를 진행했었으니...",
					L"나레이션 양반을 개조해 김두한을 1972년으로 보내버릴 최종 병기로 만든 것이었다!",
					L"",
                    HOW_TO_PLAY // Next state after the last cutscene
				);
				break;
			case HOW_TO_PLAY: // 게임 플레이 방법
				currentAppState = HowToPlay(screen);
				break;
			case GAME_PLAYING: // 메인 게임 루프
				currentAppState = GameLoop(screen);
				break;
			case CREDITS: // 크레딧
				currentAppState = Credits(screen);
				break;
			case EXIT:
				break; // 루프 탈출로 종료 처리를 수행한다
		}
	}
}

void GameApp::ClearScreen()
{
	system("cls");
}

AppState GameApp::TitleScreen(ScreenInteractive& screen)
{
	AppState nextState = TITLE_SCREEN; // Default to stay on title screen

	// 버튼
	auto button = Container::Horizontal({
		Button(
			"  게임 시작  ",
			[&] {
				nextState = INTRO_CUTSCENE;
				screen.Exit();
				return;
			},
			ButtonOption::Animated(Color::Red)
		) | center,
		Button(
			" 크레딧 ",
			[&] {
				nextState = CREDITS;
				screen.Exit();
				return;
			},
			ButtonOption::Animated(Color::Green)
		) | center,
		Button(
			" 나가기 ",
			[&] {
				nextState = EXIT;
				screen.Exit();
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
				text(L"    _____                                                                     _____  ______   ") | bold | color(Color::Red1) | center,
				text(L"   |  __ \\                                                                   |  _  ||___  /   ") | bold | color(Color::Red1) | center,
				text(L"   | |  \\/ _   _   ___   ___   _ __    __ _  ___   ___   ___   _ __    __ _  | |_| |   / /    ") | bold | color(Color::Red1) | center,
				text(L"   | | __ | | | | / _ \\ / _ \\ | '_ \\  / _` |/ __| / _ \\ / _ \\ | '_ \\  / _` | \\____ |  / /     ") | bold | color(Color::Red1) | center,
				text(L"   | |_\\ \\| |_| ||  __/| (_) || | | || (_| |\\__ \\|  __/| (_) || | | || (_| | .___/ /./ /      ") | bold | color(Color::Red1) | center,
				text(L"    \\____/ \\__, | \\___| \\___/ |_| |_| \\__, ||___/ \\___| \\___/ |_| |_| \\__, | \\____/ \\_/       ") | bold | color(Color::Red1) | center,
				text(L"            __/ |                      __/ |                           __/ |                  ") | bold | color(Color::Red1) | center,
				text(L"           |___/                      |___/                           |___/                   ") | bold | color(Color::Red1) | center,
				text(L""),
				text(L""),
				text(L"WinAPI + FTXUI + miniaudio C++ Terminal Shooting Game") | color(Color::Yellow1) | center,
				text(L""),
				text(L"홍콩 97 / 야인시대 패러디 게임") | color(Color::White) | center,
				text(L"(이 게임은 제작자의 정치적 성향과는 어떠한 연관도 없으며 그냥 웃기려고 만든 겁니다)") | color(Color::White) | center,
				text(L""),
				text(L"이 게임은 140 x 47 이상 크기의 터미널 창에서만 정상적으로 플레이할 수 있습니다") | bold | color(Color::Green) | center,
				text(L""),
				text(L""),
				button->Render() | center,
				text(L""),
			}) | border | center | color(Color::Red)
			| size(ftxui::WIDTH, Constraint::EQUAL, (GAME_WIDTH + 40) / 2)
			| size(ftxui::HEIGHT, Constraint::EQUAL, GAME_HEIGHT / 4);

		}
	);

	screen.Loop(titleScreenRenderer);

	ClearScreen();

	return nextState;
}

AppState GameApp::DrawCutscene(ScreenInteractive& screen, wstring imageName, 
	wstring textLine1, wstring textLine2, wstring textLine3, wstring textLine4, 
	AppState nextStateOnTransition)
{
	AppState nextState = INTRO_CUTSCENE; // Default to stay, will be updated if transitioned

	// 인트로 화면 생성
	auto introScreenRenderer = Renderer(
		[&] {
			return RenderSystem::RenderCutscene(imageName, textLine1, textLine2, textLine3, textLine4);
		}
	);

	// 입력 이벤트 처리
	auto introScreenComponent = CatchEvent(introScreenRenderer,
		[&](Event event) {
			if (event.character() == " ")
			{
				nextState = nextStateOnTransition;
				screen.Exit(); // 스페이스바 입력을 받으면 탈출
			}

			return true;
		}
	);

	// 화면 출력
	screen.Loop(introScreenComponent);

	ClearScreen();

	return nextState;
}

AppState GameApp::HowToPlay(ScreenInteractive& screen)
{
	AppState nextState = HOW_TO_PLAY; // Default to stay, but it should transition out.

	auto renderer = Renderer(
		[&] {
			return vbox({
				text(L""),
				text(L"H O W   T O   P L A Y") | bold | color(Color::Red1) | center,
				text(L"                                                                             "),
				text(L""),
				text(L"화살표 키 : 플레이어 캐릭터 이동") | color(Color::White) | center,
				text(L"Z 키 : 공격") | color(Color::White) | center,
				text(L"X 키 : 수류탄 투척") | color(Color::White) | center,
				text(L""),
				text(L""),
				text(L"Z키를 눌러 게임 시작") | center | color(Color::GrayDark),
				text(L"")
			}) | border | center | color(ftxui::Color::Red1)
				| size(ftxui::WIDTH, Constraint::EQUAL, (GAME_WIDTH + 40) / 2)
				| size(ftxui::HEIGHT, Constraint::EQUAL, GAME_HEIGHT / 4);
		}
	);

	// 입력 이벤트 처리
	auto screenComponent = CatchEvent(renderer,
		[&](Event event) {
			if (event == Event::z || event == Event::Z)
			{
				nextState = GAME_PLAYING;
				screen.Exit(); // 스페이스바 입력을 받으면 탈출
			}

			return true;
		}
	);

	screen.Loop(screenComponent);

	ClearScreen();

	return nextState;
}

AppState GameApp::Credits(ScreenInteractive& screen)
{
	AppState nextState = CREDITS; // Default to stay

	// 파일을 읽어들일 준비
	vector<string> lines;

	// Credit 파일을 읽어들인다
	Utility::ReadTextFile(PATH_CREDIT.data(), lines);

	int i = 7;
	while (i --> 0) lines.push_back("\n");

	// License 파일을 읽어들인다
	Utility::ReadTextFile(PATH_LICENSE.data(), lines);

	i = 7;
	while (i --> 0) lines.push_back("\n");

	// Third Party Notice 파일을 읽어들인다
	Utility::ReadTextFile(PATH_THIRD_PARTY_NOTICE.data(), lines);

	// 스크롤 상태 변수
	float scrollY = 0.0f;
	bool running = true;
	const int startPadding = 40; // 텍스트가 시작되기 전 여백 (화면 아래에서 시작)

	// 화면 생성
	auto renderer = Renderer(
		[&] {
			return RenderSystem::RenderCredits(lines, startPadding, scrollY);
		}
	);

	// 스크롤 업데이트 스레드
	std::thread scrollThread([&] {
		while (running)
		{
			// 스크롤 속도 조절 (값이 작을수록 빠름)
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			if (!running) break;

			scrollY += 1.0f; // 1줄씩 올림
			screen.Post(Event::Custom); // 화면 갱신 요청

			// 모든 크레딧이 올라가고 충분히 시간이 지나면 자동 종료
			if (scrollY > lines.size() + startPadding + 15)
			{
				nextState = TITLE_SCREEN; // Transition back to title screen
				screen.Exit();
				running = false;
			}
		}
	});

	// 입력 이벤트 처리
	auto screenComponent = CatchEvent(renderer,
		[&](Event event) {
			// Space 나 Escape 입력을 받으면 탈출
			if (event.character() == " " || event == ftxui::Event::Escape)
			{
				nextState = TITLE_SCREEN; // Transition back to title screen
				running = false;
				screen.Exit();
				return true;
			}

			// 방향키 위/아래로 수동 스크롤 속도 조절 혹은 이동 (선택 사항)
			if (event == Event::ArrowDown)
			{
				scrollY += 1.0f;
				return true;
			}
			if (event == Event::ArrowUp)
			{
				scrollY -= 1.0f;
				return true;
			}
			if (event == Event::PageDown)
			{
				scrollY += 10.0f;
				return true;
			}
			if (event == Event::PageUp)
			{
				scrollY -= 10.0f;
				return true;
			}

			return false;
		}
	);

	screen.Loop(screenComponent);


	// 정리
	running = false;
	ClearScreen();
	if (scrollThread.joinable())
	{
		scrollThread.join();
	}

	return nextState;
}

AppState GameApp::GameLoop(ScreenInteractive& screen)
{
	AppState nextState = GAME_PLAYING;

	// 게임 매니저 인스턴스
	GameManager& gameManager = GameManager::GetInstance();
	gameManager.Initialize();

	// 컴포넌트 연결
	// Renderer: 화면을 그리는 역할
	auto renderer = Renderer(
		[&] {
			return RenderSystem::RenderGameScreen();
		}
	);

	// CatchEvent: 키보드 입력을 받아 처리하는 역할
	auto component = CatchEvent(renderer,
		[&](Event event) {
			if (!gameManager.OnEvent(screen, event))
			{
				return false;
			}

			// 게임이 중단되면 종료 신호 전달
			if (!gameManager.IsRunning)
			{
				nextState = TITLE_SCREEN; // 게임이 종료되었으므로 타이틀 화면으로 복귀
			}

			return true;
		}
	);

	// 게임 로직 스레드 생성
	// UI 렌더링과 별개로 게임의 로직을 일정 간격(TICK_TIME)으로 실행
	std::thread thread(
		[&] {
			// 게임 매니저 리셋
			gameManager.Reset();

			// 시간 측정 준비
			using clock = std::chrono::steady_clock;

			auto nextFrameTime = clock::now();
			auto lastFpsTime = clock::now();
			int frames = 0;
			int logics = 0;

			// 게임 시작 시간 기록
			auto startTime = clock::now();
			auto accumulatedPauseDuration = clock::duration::zero();
			auto lastLoopTime = clock::now();

			// 게임을 재시작할 때 마다 시간 초기화
			gameManager.onResetCallback = [&]() {
				startTime = clock::now();
				accumulatedPauseDuration = clock::duration::zero();
			};

			// 게임 로직 루프
			while (gameManager.IsRunning)
			{
				auto now = clock::now();
				auto loopDelta = now - lastLoopTime;
				lastLoopTime = now;

				if (gameManager.IsGamePause)
				{
					accumulatedPauseDuration += loopDelta;
				}

				// 로직 업데이트
				// 시간이 많이 지났으면 Update를 여러 번 호출해서 게임 속도를 맞춤
				int updateCount = 0;

				while (now >= nextFrameTime && updateCount < 4) // 최대 4번까지만 따라잡기 (무한루프 방지)
				{
					gameManager.Update();
					logics++;
					nextFrameTime += TICK_TIME;
					updateCount++;
				}

				// 화면 갱신
				screen.Post(Event::Custom);
				frames++;

				// 임계 구역 설정
				{
					std::lock_guard<std::recursive_mutex> lock(gameManager.gameMutex);

					// FPS 계산 (1초마다 갱신)
					if (now - lastFpsTime >= std::chrono::milliseconds(1000))
					{
						gameManager.currentLps = logics;
						gameManager.currentFps = frames;
						logics = 0;
						frames = 0;
						lastFpsTime = now;
					}

					// 게임 진행 시간 계산
					if (!gameManager.IsGameOver && !gameManager.IsGameClear && !gameManager.IsGamePause)
					{
						chrono::duration<double> duration = (now - startTime) - accumulatedPauseDuration;
						long long durationSecond = static_cast<long long>(duration.count());
						long long mm = durationSecond / 60;
						long long ss = durationSecond % 60;

						wstringstream strStream;
						strStream << to_wstring(mm) << L":" << setw(2) << setfill(L'0') << to_wstring(ss);
					
						gameManager.gameTime = strStream.str();
					}
				}

				// 잠깐 대기 (CPU 과점유 방지)
				this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	);

	// 화면 출력
	screen.Loop(component);

	// 정리
	ClearScreen();
	if (thread.joinable())
	{
		thread.join();
	}

	return nextState;
}
