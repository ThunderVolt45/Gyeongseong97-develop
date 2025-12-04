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

		// 총 소리 출력 (일반 총알만?)
		AudioManager::GetInstance().PlayAudio(SFX_GUNFIRE.data(), 0.1f);
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

		// 폭발 효과 생성
		std::shared_ptr<Explosion> explosion = ExplosionPool::GetInstance().GetExplosion(GetCenterX(), GetCenterY(), 50, 50);
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
