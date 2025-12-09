#pragma once

#include <string>
#include <ftxui/component/screen_interactive.hpp>

class GameApp
{
public:
	GameApp();
	~GameApp();

	void Run();

private:
	void ClearScreen();
	void TitleScreen(ftxui::ScreenInteractive& screen);
	void CutsceneScreen(ftxui::ScreenInteractive& screen);
	void DrawCutscene(ftxui::ScreenInteractive& screen, std::wstring imageName, std::wstring textLine1, std::wstring textLine2, std::wstring textLine3, std::wstring textLine4);
	void HowToPlay(ftxui::ScreenInteractive& screen);
	void Credits(ftxui::ScreenInteractive& screen);
	void GameLoop(ftxui::ScreenInteractive& screen);
};
