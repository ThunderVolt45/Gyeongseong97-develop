#include "Explosion.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "ImageLoader.h"
#include "GameConstants.h"

const int SIZE_X = 40;
const int SIZE_Y = 30;

// 플라이웨이트 패턴
// 기본 크기 폭8 오브젝트는 다 같은 Sprite를 돌려쓰므로 그냥 static으로 하나만 저장하면 된다.
std::vector<Sprite> Explosion::commonSprites;

Explosion::Explosion(int x, int y)
{
	this->x = (float)(x - SIZE_X / 2);
	this->y = (float)(y - SIZE_Y / 2);

	tick = 0;
	lifeTimeTick = 120;
	animationIndex = 0;
	isCommonSize = true;

	// 폭8 효과음 재생
	AudioManager::GetInstance().PlayAudio(SFX_EXPLOSION.data(), 0.25f, false);

	// 에셋이 로드되지 않았다면 로드 (Lazy Loading)
	if (commonSprites.empty())
	{
		LoadSprites(commonSprites, SIZE_X, SIZE_Y);
	}
	
	// 기본 Sprite 설정
	if (!commonSprites.empty())
	{
		sprite = commonSprites[0];
	}
}

Explosion::Explosion(int x, int y, int w, int h)
{
	this->x = (float)(x - w / 2);
	this->y = (float)(y - h / 2);

	tick = 0;
	lifeTimeTick = 120;
	animationIndex = 0;
	isCommonSize = false;

	// 폭8 효과음 재생
	AudioManager::GetInstance().PlayAudio(SFX_EXPLOSION.data(), 0.2f, false);

	// 고유 크기를 갖는 자체 폭8 Sprite 생성
	LoadSprites(customSprites, w, h);

	// 기본 Sprite 설정
	sprite = customSprites[0];
}

void Explosion::LoadSprites(std::vector<Sprite>& sprites, int w = 40, int h = 30)
{
	// Sprites 초기화
	for (int i = 1; i <= 37; i++)
	{
		std::wstringstream ss;
		ss << L"폭8\\폭8_" << std::setw(4) << std::setfill(L'0') << i << ".png";

		Sprite sprite = ImageLoader::CreateSpriteFromImage(ss.str(), w, h);
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

	if (isCommonSize)
	{
		// 애니메이션 재생
		if (tick % 4 == 0 && !commonSprites.empty() && animationIndex < commonSprites.size() - 1)
		{
			sprite = commonSprites[++animationIndex];
		}
	}
	else
	{
		// 애니메이션 재생
		if (tick % 4 == 0 && !customSprites.empty() && animationIndex < customSprites.size() - 1)
		{
			sprite = customSprites[++animationIndex];
		}
	}
}
