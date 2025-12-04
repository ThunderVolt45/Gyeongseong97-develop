#include "Bullet.h"
#include "AudioManager.h"
#include "GameConstants.h"
#include "BulletPool.h"

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

	ftxui::Color color = ftxui::Color::Yellow;

	if (!isMine)
	{
		color = ftxui::Color::Red1;
	}

	sprite = Sprite(1, 1, { color });

	// 총 소리 출력
	AudioManager::GetInstance().PlayAudio(SFX_GUNFIRE.data(), 0.1f);
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

void Bullet::OnDestroy(std::shared_ptr<GameObject> self)
{
	if (auto bullet = std::dynamic_pointer_cast<Bullet>(self))
	{
		BulletPool::GetInstance().ReturnBullet(bullet);
	}
}