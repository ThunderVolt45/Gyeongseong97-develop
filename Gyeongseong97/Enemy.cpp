#include "Enemy.h"

#include "GameManager.h"
#include "Bullet.h"
#include "ObjectPool.h"
#include "Explosion.h"
#include "GameConstants.h"
#include "Enums.h"

#pragma region Constructer & Destroyer

Enemy::Enemy()
{

}

Enemy::Enemy(int x, int y, int health, float speed, int killScore)
{
	type = ObjectType::Enemy;

	this->health = health;
	this->speed = speed;
	this->killScore = killScore;

	originalSprite = ImageLoader::CreateSpriteFromImage(SPR_INSTIGATED.data(), 18, 36);
	sprite = originalSprite;

	hitSprite = ImageLoader::CreateHitSprite(sprite);

	this->x = (float)(x - (sprite.sizeX / 2));
	this->y = (float)(y - (sprite.sizeY / 2));
}

#pragma endregion

#pragma region Protected

void Enemy::ProcessHitEffect()
{
	if (hitEffectTick == 0) return;

	hitEffectTick--;

	if (hitEffectTick > 0)
	{
		sprite = hitSprite;
	}
	else
	{
		sprite = originalSprite;
	}
}

void Enemy::Destroy()
{
	GameManager& gameManager = GameManager::GetInstance();

	std::shared_ptr<Explosion> explosion = ObjectPool<Explosion>::GetInstance().Get(GetCenterX(), GetCenterY());
	gameManager.CreateGameObject(explosion, TargetLayer::Background);
	gameManager.DestroyGameObject(this);
	gameManager.score += killScore;
}

#pragma endregion

#pragma region Public

void Enemy::TakeDamage(float damage)
{
	health -= damage;
	hitEffectTick = 2;

	if (health <= 0)
	{
		Destroy();
	}
}

void Enemy::Update()
{
	ProcessHitEffect();

	y += speed;
}

void Enemy::OnCollision(GameObject& other)
{
	// 만약 Bullet과 충돌했다면
	Bullet* bullet = dynamic_cast<Bullet*>(&other);
	if (bullet)
	{
		// 플레이어가 쏜 총알에만 반응
		if (bullet->isPlayer)
		{
			TakeDamage(bullet->GetDamage());

			// 히트 당 기본 점수
			GameManager::GetInstance().score += SCORE_FOR_HIT;
			
			// 총알 파괴
			bullet->Destroy();
		}

		return;
	}

	// 만약 Player와 충돌했다면
	Player* player = dynamic_cast<Player*>(&other);
	if (player)
	{
		TakeDamage(COLLISION_DAMAGE);
	}
}

#pragma endregion