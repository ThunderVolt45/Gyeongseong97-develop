#include "GameManager.h"
#include "AudioManager.h"
#include "GameConstants.h"
#include "Army.h"
#include "Bullet.h"
#include "ObjectPool.h"
#include "Enums.h"

#pragma region Constructer& Destroyer

Army::Army()
{

}

Army::Army(int x, int y, int health, float speed, int killScore)
{
	type = ObjectType::Enemy;

	cooldown = 30; // 등장하자마자 사격하지 못하게 한다

	this->health = health;
	this->speed = speed;
	this->killScore = killScore;

	originalSprite = ImageLoader::CreateSpriteFromImage(SPR_ARMY.data(), 18, 36);
	sprite = originalSprite;

	hitSprite = ImageLoader::CreateHitSprite(sprite);

	this->x = (float)(x - (sprite.sizeX / 2));
	this->y = (float)(y - (sprite.sizeY / 2));
}

#pragma endregion

#pragma region Public

void Army::Update()
{
	ProcessHitEffect();

	y += speed;
	if (cooldown > 0) cooldown -= 1;

	Attack();
}

#pragma endregion

#pragma region AI (?)

void Army::Attack()
{
	// 아직 쿨다운 중이라면 중단
	if (cooldown > 0) return;

	GameManager& gameManager = GameManager::GetInstance();

	// 플레이어와 인민군 사이의 방향 벡터를 구한다 (Bullet은 x -= speedX 이므로 this - player로 계산)
	float dx = GetCenterX() - gameManager.player.GetCenterX();
	float dy = GetCenterY() - gameManager.player.GetCenterY();

	// 정규화
	float dist = std::sqrt(dx * dx + dy * dy);
	if (dist > 0)
	{
		dx /= dist;
		dy /= dist;
	}

	// 속도 적용
	dx *= speed * 5;
	dy *= speed * 5;

	// 총알 생성
	std::shared_ptr<Bullet> bullet = ObjectPool<Bullet>::GetInstance().Get(
		GetCenterX(), 
		GetCenterY(), 
		dx, 
		dy, 
		false
	);

	// 발사!
	GameManager::GetInstance().CreateGameObject(bullet, TargetLayer::Foreground);
	AudioManager::GetInstance().PlayAudio(SFX_GUNFIRE.data(), 0.2f);

	cooldown += 90; // 90틱 마다 한번 공격
}

#pragma endregion
