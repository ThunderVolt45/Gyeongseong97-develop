#include "Bullet.h"
#include "AudioManager.h"
#include "GameConstants.h"
#include "BulletPool.h"
#include "GameManager.h"
#include "ExplosionPool.h"
#include "Explosion.h"

Bullet::Bullet()
{

}

Bullet::Bullet(int x, int y, float speedX, float speedY, bool isMine, int damage)
{
	Reset(x, y, speedX, speedY, isMine, damage);
}

void Bullet::Reset(int x, int y, float speedX, float speedY, bool isMine, int damage, bool isExplosive)
{
	this->x = x;
	this->y = y;
	this->speedX = speedX;
	this->speedY = speedY;
	this->isPlayer = isMine;
	this->damage = damage;
	this->isExplosive = isExplosive;

	AudioManager& audioManager = AudioManager::GetInstance();

	if (isExplosive)
	{
		ftxui::Color c = ftxui::Color::DarkRed;

		if (!isMine)
		{
			c = ftxui::Color::RosyBrown;
		}

		sprite = Sprite(4, 4,
			{
				c, c, c, c,
				c, c, c, c,
				c, c, c, c,
				c, c, c, c,
			});
	}
	else
	{
		ftxui::Color color = ftxui::Color::Yellow;

		if (!isMine)
		{
			color = ftxui::Color::Red1;
		}

		sprite = Sprite(1, 1, { color });
	}
}

void Bullet::Update()
{
	x -= speedX;
	y -= speedY;
}

int Bullet::GetDamage()
{
	return damage;
}

void Bullet::Destroy()
{
	if (isExplosive)
	{
		GameManager& gameManager = GameManager::GetInstance();
		BulletPool& pool = BulletPool::GetInstance();

		// 파편 효과 생성 (총알)
		// 8방향으로 총알 발사
		for (int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				if (x == 0 && y == 0) continue;

				std::shared_ptr<Bullet> bullet = pool.GetBullet(
					GetCenterX(),
					GetCenterY(),
					6.0f * x,
					6.0f * y,
					isPlayer
				);

				gameManager.CreateGameObject(bullet);
			}
		}

		// 폭발 효과 생성 (데미지 전달)
		std::shared_ptr<Explosion> explosion = 
			ExplosionPool::GetInstance().GetExplosion(GetCenterX(), GetCenterY(), 60, 45, damage);

		gameManager.CreateGameObject(explosion, false);
	}

	GameObject::Destroy();
}

void Bullet::OnDestroy(std::shared_ptr<GameObject> self)
{
	if (auto bullet = std::dynamic_pointer_cast<Bullet>(self))
	{
		BulletPool::GetInstance().ReturnBullet(bullet);
	}
}
