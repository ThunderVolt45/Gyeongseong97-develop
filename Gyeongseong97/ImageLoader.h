#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ftxui/screen/color.hpp>

struct RawImage
{
	int width;
	int height;
	int n_channels;
	unsigned char* data;
};

struct Sprite
{
	int sizeX = 0;
	int sizeY = 0;
	std::vector<ftxui::Color> colors = std::vector<ftxui::Color>();
};

class ImageLoader
{
private:
	static std::map<std::string, RawImage> cacheImages;

public:
	static Sprite CreateSpriteFromImage(std::wstring fileName, int sizeX, int sizeY);
};

