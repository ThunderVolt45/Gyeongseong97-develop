#include "Explosion.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "ImageLoader.h"

const int SIZE_X = 32;
const int SIZE_Y = 24;

// 플라이웨이트 패턴
// 어차피 폭8 오브젝트는 다 같은 Sprite를 돌려쓰므로 그냥 static으로 하나만 저장하면 된다.
std::vector<Sprite> Explosion::commonSprites;

Explosion::Explosion(int x, int y)
{
	this->x = x;
	this->y = y;

	tick = 0;
	lifeTimeTick = 120;
	animationIndex = 0;

	// 효과음 재생
	AudioManager::GetInstance().PlayAudio(L"sfx_explosion.mp3", 0.25f, false);

	// 에셋이 로드되지 않았다면 로드 (Lazy Loading)
	if (commonSprites.empty())
	{
		LoadAssets();
	}

	// 기본 Sprite 설정
	Sprite sprite = ImageLoader::CreateSpriteFromImage(L"폭8_0001.png", SIZE_X, SIZE_Y);
}

void Explosion::LoadAssets()
{
	// Sprites 초기화
	for (int i = 0; i < 36; i++)
	{
		std::wstringstream ss;
		ss << L"폭8\\폭8_" << std::setw(4) << std::setfill(L'0') << i << ".png";
		Sprite sprite = ImageLoader::CreateSpriteFromImage(ss.str(), SIZE_X, SIZE_Y);
		commonSprites.push_back(sprite);
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
	if (tick % 4 == 0 && !commonSprites.empty() && animationIndex < commonSprites.size() - 1)
	{
		sprite = commonSprites[++animationIndex];
	}
}
