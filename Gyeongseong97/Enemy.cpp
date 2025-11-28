#include "GameManager.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Explosion.h"

#pragma region Constructer & Destroyer

Enemy::Enemy()
{

}

Enemy::Enemy(int x, int y, int health, float speed, int killScore) : GameObject(x, y)
{
	this->health = health;
	this->speed = speed;
	this->killScore = killScore;

	using color = ftxui::Color;

	sprite = ImageLoader::CreateSpriteFromImage(L"enemy_Instigated.png", 24, 24);
}

Enemy::Enemy(int x, int y, int w, int h, std::wstring spriteName, int health = 1, float speed = 0.5f, int killScore = 100) : GameObject(x, y, w, h, spriteName)
{
	this->health = health;
	this->speed = speed;
	this->killScore = killScore;
}

#pragma endregion

#pragma region Private

void Enemy::Destroy()
{
	std::shared_ptr<Explosion> explosion(new Explosion(GetCenterX(), GetCenterY()));
	GameManager::GetInstance().CreateGameObject(explosion, false);
	GameManager::GetInstance().DestroyGameObject(this);
	GameManager::GetInstance().score += killScore;
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