#include "GameManager.h"
#include "AudioManager.h"
#include "GameConstants.h"
#include "Carmikaze.h"
#include "Explosion.h"
#include "ExplosionPool.h"

#pragma region Constructer & Destroyer

Carmikaze::Carmikaze()
{

}

Carmikaze::Carmikaze(int x, int y, int health, float speed, int killScore)
{
	this->health = health;
	this->speed = speed;
	this->killScore = killScore;
	
	// 스프라이트 로드
	originalSprite = ImageLoader::CreateSpriteFromImage(SPR_CARMIKAZE.data(), 60, 30);

	// 만약 오른쪽에서 생성된다면 sprite를 뒤집어준다
	if (x > GAME_WIDTH / 2)
	{
		this->direction = -1;
		originalSprite = ImageLoader::FlipSpriteX(originalSprite);
	}
	else
	{
		this->direction = 1;
	}

	sprite = originalSprite;
	hitSprite = ImageLoader::CreateHitSprite(sprite);

	this->x = (float)(x - (sprite.sizeX / 2));
	this->y = (float)(y - (sprite.sizeY / 2));

	// 등장 SFX
	AudioManager::GetInstance().PlayAudio(SFX_CAR.data());
}

#pragma endregion

#pragma region Protected

void Carmikaze::Destroy()
{
	GameManager& gameManager = GameManager::GetInstance();

	std::shared_ptr<Explosion> explosion = ExplosionPool::GetInstance().GetExplosion(GetCenterX(), GetCenterY(), 60, 45);
	gameManager.CreateGameObject(explosion, false);
	gameManager.DestroyGameObject(this);
	gameManager.score += killScore;
}

#pragma endregion

#pragma region Public

void Carmikaze::Update()
{
	ProcessHitEffect();

	x += speed * direction;
}

#pragma endregion
