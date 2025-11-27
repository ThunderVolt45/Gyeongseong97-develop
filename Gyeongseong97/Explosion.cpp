#include "Explosion.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "ImageLoader.h"

const int SIZE_X = 32;
const int SIZE_Y = 24;

Explosion::Explosion(int x, int y)
{
	this->x = x;
	this->y = y;

	tick = 0;
	lifeTimeTick = 120;
	animationIndex = 0;

	AudioManager::GetInstance().PlayAudio(L"sfx_explosion.mp3",  0.5f);
	sprite = ImageLoader::CreateSpriteFromImage(L"폭8/폭8_0003.png", SIZE_X, SIZE_Y);

	// Sprites 초기화
	for (int i = 0; i < 36; i++)
	{
		std::wstringstream ss;
		ss << L"폭8\\폭8_" << std::setw(4) << std::setfill(L'0') << i << ".png";
		Sprite sprite = ImageLoader::CreateSpriteFromImage(ss.str(), SIZE_X, SIZE_Y);
		sprites.push_back(sprite);
	}
}

void Explosion::Update()
{
	tick += 1;

	if (tick > lifeTimeTick)
	{
		GameManager::GetInstance().DestroyGameObject(this);
		return;
	}

	// 애니메이션 재생
	if (tick % 4 == 0 && animationIndex < sprites.size() - 1)
	{
		sprite = sprites[++animationIndex];
	}
}
