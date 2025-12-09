#pragma once

#include <string>
#include <ftxui/component/screen_interactive.hpp>

enum AppState {
	TITLE_SCREEN,
	INTRO_CUTSCENE,
	HOW_TO_PLAY,
	GAME_PLAYING,
	CREDITS,
	EXIT
};

class GameApp
{
public:
	GameApp();
	~GameApp();

	void Run();

private:
	AppState currentAppState;

	void ClearScreen();
	AppState TitleScreen(ftxui::ScreenInteractive& screen);
	AppState DrawCutscene(ftxui::ScreenInteractive& screen, std::wstring imageName, std::wstring textLine1, std::wstring textLine2, std::wstring textLine3, std::wstring textLine4, AppState nextStateOnTransition);
	AppState HowToPlay(ftxui::ScreenInteractive& screen);
	AppState Credits(ftxui::ScreenInteractive& screen);
	AppState GameLoop(ftxui::ScreenInteractive& screen);
};

