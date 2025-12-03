#include "Explosion.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "ImageLoader.h"
#include "GameConstants.h"

// 플라이웨이트 패턴
// 기본 크기 폭8 오브젝트는 다 같은 Sprite를 돌려쓰므로 그냥 static으로 하나만 저장하면 된다.
std::vector<Sprite> Explosion::commonSprites;

Explosion::Explosion(int x, int y, int w, int h)
{
	Reset(x, y, w, h);
}

void Explosion::Reset(int x, int y, int w, int h)
{
	this->x = (float)(x - w / 2);
	this->y = (float)(y - h / 2);

	tick = 0;
	lifeTimeTick = 120;
	animationIndex = 0;
	isCommonSize = (w == EXPLOSION_DEFAULT_SIZE_X && h == EXPLOSION_DEFAULT_SIZE_Y) ? true : false;
	customSprites.clear();

	// 폭8 효과음 재생
	AudioManager::GetInstance().PlayAudio(SFX_EXPLOSION.data(), 0.2f, false);

	// Sprite 설정
	SetSprites(w, h);
}

void Explosion::SetSprites(int w, int h)
{
	if (isCommonSize)
	{
		// 에셋이 로드되지 않았다면 로드 (Lazy Loading)
		if (commonSprites.empty())
		{
			LoadSprites(commonSprites, EXPLOSION_DEFAULT_SIZE_X, EXPLOSION_DEFAULT_SIZE_Y);
		}

		// 기본 Sprite 설정
		if (!commonSprites.empty())
		{
			sprite = commonSprites[0];
		}
	}
	else
	{
		// 고유 크기를 갖는 자체 폭8 Sprite 생성
		LoadSprites(customSprites, w, h);

		// 기본 Sprite 설정
		sprite = customSprites[0];
	}
}

void Explosion::LoadSprites(std::vector<Sprite>& sprites, int w = 40, int h = 30)
{
	// Sprites 초기화
	for (int i = 1; i <= 37; i++)
	{
		std::wstringstream ss;
		ss << SPR_EXPLOSION_PREFIX.data() << std::setw(4) << std::setfill(L'0') << i << POSTFIX_PNG.data() ;

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
