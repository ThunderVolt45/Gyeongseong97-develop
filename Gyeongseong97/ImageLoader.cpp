#pragma execution_character_set( "utf-8" )

#include "ImageLoader.h"
#include "Utility.h"

#include <iostream>
#include <string>
#include <vector>

#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8

#include <stb_image.h>

const static std::wstring IMAGE_PATH = L"\\Sprites\\";

Sprite ImageLoader::CreateSpriteFromImage(std::wstring fileName, int sizeX, int sizeY)
{
	Sprite sprite;

	// 이미지 경로 계산
	std::vector<wchar_t> pathBuffer(MAX_PATH);
	DWORD ret = GetCurrentDirectoryW(MAX_PATH, pathBuffer.data());

	if (ret == 0)
	{
		std::wcerr << L"Error: 이미지 경로를 가져오는데 실패했습니다." << std::endl;
		return sprite;
	}
	else if (ret == pathBuffer.size())
	{
		std::wcerr << L"Warning: 이미지 경로가 너무 깁니다. 더 큰 버퍼가 필요합니다." << std::endl;
	}

	auto strFilePath = Utility::ConvertWideToUtf8(pathBuffer.data()) + Utility::ConvertWideToUtf8((IMAGE_PATH + fileName).c_str());
	const char* filePath = strFilePath.c_str();

	int width, height, n_channels;

	// 이미지 파일의 픽셀 데이터를 메모리로 로드한다.
	unsigned char* data = stbi_load(filePath, &width, &height, &n_channels, 4);

	// 이미지 로딩에 실패했다면
	if (data == nullptr)
	{
		std::wcerr << L"stb: 이미지를 로드할 수 없습니다. " << filePath << L" 파일이 존재하는지 확인하세요." << std::endl;
		return sprite;
	}

	// Sprite 생성 준비
	sprite.sizeX = sizeX;
	sprite.sizeY = sizeY;

	// Sprite의 모든 픽셀을 순회
	for (int y = 0; y < sizeY; y++)
	{
		for (int x = 0; x < sizeX; x++)
		{
			// Box Sampling (Averaging) for filtering
			int startX = x * width / sizeX;
			int endX = (x + 1) * width / sizeX;
			int startY = y * height / sizeY;
			int endY = (y + 1) * height / sizeY;

			if (endX <= startX) endX = startX + 1;
			if (endY <= startY) endY = startY + 1;
			if (endX > width) endX = width;
			if (endY > height) endY = height;

			unsigned int r_sum = 0, g_sum = 0, b_sum = 0, a_sum = 0;
			int count = 0;

			for (int iy = startY; iy < endY; ++iy)
			{
				for (int ix = startX; ix < endX; ++ix)
				{
					unsigned char* pixel_offset = data + (ix + width * iy) * 4;
					r_sum += pixel_offset[0];
					g_sum += pixel_offset[1];
					b_sum += pixel_offset[2];
					a_sum += pixel_offset[3];
					count++;
				}
			}

			uint8_t r = 0, g = 0, b = 0, a = 0;
			if (count > 0)
			{
				r = static_cast<uint8_t>(r_sum / count);
				g = static_cast<uint8_t>(g_sum / count);
				b = static_cast<uint8_t>(b_sum / count);
				a = static_cast<uint8_t>(a_sum / count);
			}

			// Sprite 구조체에 색상 정보를 넣는다.
			sprite.colors.push_back(ftxui::Color(r, g, b, a));
		}
	}

	// 메모리 해제
	stbi_image_free(data);

	// Sprite 반환
	return sprite;
}