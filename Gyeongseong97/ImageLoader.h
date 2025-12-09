#pragma once
#include <ftxui/screen/color.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <map>

struct Sprite
{
	int sizeX = 0;
	int sizeY = 0;
	std::vector<ftxui::Color> colors = std::vector<ftxui::Color>();
};

class ImageLoader
{
private:
	static std::map<std::string, Sprite> spriteCache;

public:
	static Sprite CreateSpriteFromImage(std::wstring fileName, int sizeX, int sizeY);
	static Sprite FlipSpriteX(Sprite spriteToFilp);
	static Sprite CreateHitSprite(Sprite originalSprite);
};