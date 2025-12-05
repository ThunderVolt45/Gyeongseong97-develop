#pragma once
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/component/component.hpp>

#include "GameObject.h"

class RenderSystem
{
private:
	static std::string errorMessage;
	static bool isErrorActive;

	static void DrawObjectSprite(ftxui::Canvas& canvas, const GameObject& object);

public:
	static ftxui::Element Render();
	static void ShowErrorMessage(const std::string& message);
	static void ClearErrorMessage();
};

