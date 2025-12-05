#pragma execution_character_set( "utf-8" )

#include "ImageLoader.h"
#include "RenderSystem.h"
#include "Utility.h"
#include "GameConstants.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include <stb_image.h>

// 정적 멤버 변수 정의
std::map<std::string, Sprite> ImageLoader::spriteCache;

Sprite ImageLoader::CreateSpriteFromImage(std::wstring fileName, int sizeX, int sizeY)
{
	// 캐시 키 생성 (파일이름_너비_높이)
	std::string cacheKey = Utility::ConvertWideToUtf8(fileName.c_str()) + "_" + std::to_string(sizeX) + "_" + std::to_string(sizeY);

	// 캐시에 있으면 반환
	if (spriteCache.contains(cacheKey))
	{
		return spriteCache[cacheKey];
	}
	
	// 이미지 경로를 구한다
	filesystem::path currentPath = filesystem::current_path();

	auto strFilePath = Utility::ConvertWideToUtf8(currentPath.c_str()) + Utility::ConvertWideToUtf8((IMAGE_PATH.data() + fileName).c_str());
	const char* filePath = strFilePath.c_str();

	// 이미지 로드!
	int width, height, n_channels;
	unsigned char* data = stbi_load(filePath, &width, &height, &n_channels, 4);

	// Sprite 생성
	Sprite sprite;

	// 이미지 로딩에 실패했다면 핑크색으로 채워서 보낸다
	if (!data)
	{
		RenderSystem::ShowErrorMessage("ImageLoader : " + strFilePath + " 파일을 불러오는데 실패했습니다.");
		
		sprite.sizeX = sizeX;
		sprite.sizeY = sizeY;
		for (int y = 0; y < sizeY; y++)
		{
			for (int x = 0; x < sizeX; x++)
			{
				sprite.colors.push_back(ftxui::Color::Pink1);
			}
		}

		// 실패한 경우 캐시하지 않거나, 빈 스프라이트를 캐시할 수도 있음. 
		// 여기서는 매번 재시도하도록 캐시하지 않음.
		return sprite;
	}

	// Sprite 생성 준비
	sprite.sizeX = sizeX;
	sprite.sizeY = sizeY;
	// 미리 메모리 예약
	sprite.colors.reserve(sizeX * sizeY);

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

	// 생성된 스프라이트를 캐시에 저장
	spriteCache[cacheKey] = sprite;

	// Sprite 반환
	return sprite;
}

Sprite ImageLoader::FlipSpriteX(Sprite spriteToFlip)
{
	Sprite newSprite;
	newSprite.sizeX = spriteToFlip.sizeX;
	newSprite.sizeY = spriteToFlip.sizeY;
	newSprite.colors.reserve(spriteToFlip.colors.size()); // Reserve space for efficiency

	// Sprite의 모든 픽셀을 순회
	for (int y = 0; y < spriteToFlip.sizeY; y++)
	{
		for (int x = 0; x < spriteToFlip.sizeX; x++)
		{
			// 원본 스프라이트의 (sizeX - 1 - x, y) 위치의 픽셀을 가져와
			// 새로운 스프라이트의 (x, y) 위치에 넣는다.
			// 인덱스 계산: x + y * width
			newSprite.colors.push_back(spriteToFlip.colors[(spriteToFlip.sizeX - 1 - x) + y * spriteToFlip.sizeX]);
		}
	}

	return newSprite;
}

Sprite ImageLoader::CreateHitSprite(Sprite originalSprite)
{
	Sprite newSprite;
	newSprite.sizeX = originalSprite.sizeX;
	newSprite.sizeY = originalSprite.sizeY;
	newSprite.colors.reserve(originalSprite.colors.size()); // Reserve space for efficiency

	// Sprite의 모든 픽셀을 순회
	for (int i = 0; i < originalSprite.colors.size(); i++)
	{
		ftxui::Color color = ftxui::Color::RGBA(255, 0, 0, originalSprite.colors[i].IsOpaque() ? 255 : 0);
		newSprite.colors.push_back(color);
	}

	return newSprite;
}