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

void Bullet::Reset(int x, int y, float speedX, float speedY, bool isMine, int damage)
{
	this->x = x;
	this->y = y;
	this->speedX = speedX;
	this->speedY = speedY;
	this->isPlayer = isMine;
	this->damage = damage;
	this->isExplosive = isExplosive;

	// Clear custom behaviors
	onUpdate = nullptr;
	onDestroy = nullptr;

	AudioManager& audioManager = AudioManager::GetInstance();

	ftxui::Color color = ftxui::Color::Yellow;

	if (!isMine)
	{
		color = ftxui::Color::Red1;
	}

	sprite = Sprite(1, 1, { color });
}

void Bullet::SetCustomBehavior(Sprite sprite, std::function<void(Bullet*)> onUpdate = nullptr, std::function<void(Bullet*)> onDestroy = nullptr)
{
	this->sprite = sprite;
	this->onUpdate = onUpdate;
	this->onDestroy = onDestroy;
}

void Bullet::Update()
{
	if (onUpdate)
	{
		onUpdate(this);
	}
	else
	{
		x -= speedX;
		y -= speedY;
	}
}

int Bullet::GetDamage()
{
	return damage;
}

void Bullet::Destroy()
{
	if (onDestroy)
	{
		onDestroy(this);
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
