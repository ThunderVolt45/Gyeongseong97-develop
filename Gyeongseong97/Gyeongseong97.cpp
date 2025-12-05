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
#include "RenderSystem.h"
#include "GameConstants.h"
#include "Utility.h"
#include "AudioManager.h"
#include "ImageLoader.h"

using namespace ftxui;
using namespace std;

#pragma region Declarations
void TitleScreen(ScreenInteractive& screen);
void DrawCutscene(ScreenInteractive& screen, wstring imageName, wstring textLine1, wstring textLine2, wstring textLine3, wstring textLine4);
void GameLoop(ScreenInteractive& screen);
#pragma endregion

void ClearScreen()
{
	cout << "\033[2J\033[H";
	cout.flush();
}

int main()
{
	// 유니코드 및 콘솔 출력 설정 (UTF-8)
	SetConsoleOutputCP(CP_UTF8);

	// 오디오 엔진 초기화
	AudioManager& audioManager = AudioManager::GetInstance();
	audioManager.PlayAudio(BGM_MAIN.data(), BGM_VOULME, true);

	// 화면 정리
	ClearScreen();

	// FTXUI Screen 생성
	auto screen = ScreenInteractive::FitComponent();

	// 타이틀 화면 출력
	TitleScreen(screen);

	// 인트로 컷신 출력
	DrawCutscene(screen,
		L"intro_sy.png",
		L"",
		L"1949년이 다가왔다.",
		L"X같이 못생긴 심영이 학생과 시민들을 빨갱이로 만들기 시작했다!",
		L""
		);

	DrawCutscene(screen,
		L"intro_reds.png",
		L"",
		L"경성이 빨갱이로 가득차버렸다! 경성은 엉망이 되었다!",
		L"그리하여 비밀 결사 백의사에서는 백색 테러리스트 김두한을 불렀다.",
		L""
	);

	DrawCutscene(screen,
		L"intro_kdh.png",
		L"",
		L"김두한은 조선의 주먹 황제이자 백색 테러주의자다.",
		L"1972억 공산당 빨갱이들을 모조리 시체로 결☆ 정 시킬 것이다!",
		L""
	);

	DrawCutscene(screen,
		L"intro_narration.png",
		L"",
		L"그러나 공산당에서는 이미 비밀 연구를 진행했었으니...",
		L"나레이션 양반을 개조해 김두한을 1972년으로 보내버릴 최종 병기로 만든 것이었다!",
		L""
	);

	DrawCutscene(screen,
		L"intro.png",
		L"조 작 법",
		L"방향키 : 플레이어 상하좌우 이동",
		L"Space : 사격 (연사)",
		L""
		);

	// 게임 시작
	GameLoop(screen);

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
				text(L"WinAPI + FTXUI + miniaudio C++ 콘솔 게임 포트폴리오") | color(Color::Yellow) | center,
				text(L""),
				text(L"홍콩 97 / 야인시대 패러디 게임") | color(Color::White) | center,
				text(L"(이 게임은 제작자의 정치적 성향과는 어떠한 연관도 없으며 그냥 웃기려고 만든 겁니다)") | color(Color::White) | center,
				text(L""),
				text(L"이 게임은 140 x 47 이상 크기의 터미널 창에서만 정상적으로 플레이할 수 있습니다") | bold | color(Color::Green) | center,
				text(L""),
				text(L""),
				button->Render() | center,
				text(L""),
			}) | border | center | color(Color::Red) | size(ftxui::WIDTH, Constraint::EQUAL, (GAME_WIDTH + 40) / 2);
		}
	);

	screen.Loop(titleScreenRenderer);

	// 화면 정리
	ClearScreen();
}

void DrawCutscene(ScreenInteractive& screen, wstring imageName, wstring textLine1, wstring textLine2, wstring textLine3, wstring textLine4)
{
	// 캔버스 생성
	int canvasWidth = GAME_WIDTH + 40;
	int canvasHeight = GAME_HEIGHT - 20;
	auto canvas = ftxui::Canvas(canvasWidth, canvasHeight);

	// Sprite 로드
	Sprite sprite = ImageLoader::CreateSpriteFromImage(imageName, canvasWidth, canvasHeight);
	
	// 캔버스에 Sprite 그리기
	for (int y = 0; y < canvasHeight; y++)
	{
		for (int x = 0; x < canvasWidth; x++)
		{
			// Canvas에 색상으로 점(블록)을 찍는다.
			canvas.DrawBlock(x, y, true, sprite.colors[x + (y * canvasWidth)]);
		}
	}

	// 인트로 화면 생성
	auto canvasElement = ftxui::canvas(std::move(canvas));
	auto introScreenRenderer = Renderer(
		[&] {
			return vbox({
				canvasElement | center,
				vbox({
					text(textLine1) | center,
					text(textLine2) | center,
					text(textLine3) | center,
					text(textLine4) | center,
					text(L"스페이스바를 눌러 계속...") | align_right | color(Color::GrayDark)
				}) | border | size(ftxui::WIDTH, Constraint::EQUAL, (GAME_WIDTH + 40) / 2)
			});
		}
	);

	// 입력 이벤트 처리
	auto introScreenComponent = CatchEvent(introScreenRenderer,
		[&](Event event) {
			if (event.character() == " ")
			{
				screen.Exit(); // 스페이스바 입력을 받으면 탈출
			}

			return true;
		}
	);

	// 화면 출력
	screen.Loop(introScreenComponent);
	
	// 화면 정리
	ClearScreen();
}

void GameLoop(ScreenInteractive& screen)
{
	// 게임 매니저 인스턴스
	GameManager& gameManager = GameManager::GetInstance();
	gameManager.Initialize();

	// 컴포넌트 연결
	// Renderer: 화면을 그리는 역할
	auto renderer = Renderer(
		[&] {
			return RenderSystem::Render();
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
	// UI 렌더링과 별개로 게임의 로직을 일정 간격(TICK_TIME)으로 실행
	std::thread thread(
		[&] {
			using clock = std::chrono::steady_clock;

			auto nextFrameTime = clock::now();
			auto lastFpsTime = clock::now();
			int frames = 0;
			int logics = 0;

			// 게임 시작 시간
			auto startTime = clock::now();

			gameManager.onResetCallback = [&]() {
				startTime = clock::now();
			};

			while (gameManager.IsRunning)
			{
				auto now = clock::now();

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
				if (!gameManager.IsGameOver && !gameManager.IsGameClear)
				{
					chrono::duration<double> duration = now - startTime;
					long long durationSecond = static_cast<long long>(duration.count());
					long long mm = durationSecond / 60;
					long long ss = durationSecond % 60;

					wstringstream strStream;
					strStream << to_wstring(mm) << L":" << setw(2) << setfill(L'0') << to_wstring(ss);
					gameManager.gameTime = strStream.str();
				}

				// 잠깐 대기 (CPU 과점유 방지)
				this_thread::sleep_for(std::chrono::milliseconds(1));
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
