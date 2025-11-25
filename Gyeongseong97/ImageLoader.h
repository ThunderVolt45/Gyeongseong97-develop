#pragma once
#include <iostream>
#include <vector>
#include <ftxui/screen/color.hpp>

struct Sprite
{
	int sizeX = 0;
	int sizeY = 0;
	std::vector<ftxui::Color> colors = std::vector<ftxui::Color>();
};

class ImageLoader
{
public:
	static Sprite CreateSpriteFromImage(std::wstring fileName, int sizeX, int sizeY);
};

