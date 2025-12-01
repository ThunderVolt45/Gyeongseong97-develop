#include "GameManager.h"
#include "GameConstants.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Explosion.h"

#pragma region Constructer & Destroyer

Enemy::Enemy()
{

}

Enemy::Enemy(int x, int y, int health, float speed, int killScore)
{
	this->health = health;
	this->speed = speed;
	this->killScore = killScore;

	sprite = ImageLoader::CreateSpriteFromImage(L"enemy_Instigated.png", 12, 36);

	this->x = (float)(x - (sprite.sizeX / 2));
	this->y = (float)(y - (sprite.sizeY / 2));
}

#pragma endregion

#pragma region Protected

void Enemy::Destroy()
{
	GameManager& gameManager = GameManager::GetInstance();

	std::shared_ptr<Explosion> explosion(new Explosion(GetCenterX(), GetCenterY()));
	gameManager.CreateGameObject(explosion, false);
	gameManager.DestroyGameObject(this);
	gameManager.score += killScore;
}

#pragma endregion

#pragma region Public

void Enemy::Update()
{
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
			health -= 1;

			// 히트 당 기본 점수
			GameManager::GetInstance().score += SCORE_FOR_HIT;
			
			// 총알 파괴
			GameManager::GetInstance().DestroyGameObject(bullet);

			// 체력이 다 닳았으면 파괴
			if (health <= 0)
			{
				Destroy();
			}
		}
		return;
	}

	// 만약 Player와 충돌했다면
	Player* player = dynamic_cast<Player*>(&other);
	if (player)
	{
		health -= 0.1f;

		// 체력이 다 닳았으면 파괴
		if (health <= 0)
		{
			Destroy();
		}

		return;
	}
}

#pragma endregion