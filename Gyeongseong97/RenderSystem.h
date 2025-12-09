#pragma once
#include "GameObject.h"

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/component/component.hpp>

class RenderSystem
{
private:
	static std::string errorMessage;
	static bool isErrorActive;
	static long long errorDisplayStartTime; // New member

	static void DrawObjectSprite(ftxui::Canvas& canvas, const GameObject& object);

public:
	static ftxui::Element RenderGameScreen();
	static ftxui::Element RenderCutscene(wstring imageName, wstring textLine1, wstring textLine2, wstring textLine3, wstring textLine4);
	static ftxui::Element RenderCredits(vector<string> lines, int startPadding, float& scrollY);
	static void ShowErrorMessage(const std::string& message);
	static void ClearErrorMessage();
};

