#include "BulletPool.h"
#include "ExplosionPool.h"

BulletPool::BulletPool()
{

}

BulletPool& BulletPool::GetInstance()
{
	static BulletPool instance;
	return instance;
}

std::shared_ptr<Bullet> BulletPool::GetBullet(
	int x, int y, float speedX, float speedY, bool isMine, int damage, int lifeTick)
{
	if (pool.empty())
	{
		auto bullet = std::make_shared<Bullet>();
		bullet->Reset(x, y, speedX, speedY, isMine, damage, lifeTick);
		return bullet;
	}
	else
	{
		std::shared_ptr<Bullet> bullet = pool.back();
		pool.pop_back();

		bullet->Reset(x, y, speedX, speedY, isMine, damage, lifeTick);
		return bullet;
	}
}

std::shared_ptr<Bullet> BulletPool::GetCustomBullet(
	int x, int y, float speedX, float speedY, bool isMine, int damage, int lifeTick,
	Sprite sprite, std::function<void(Bullet*)> onUpdate, std::function<void(Bullet*)> onDestroy)
{
	auto bullet = GetBullet(x, y, speedX, speedY, isMine, damage, lifeTick);
	bullet->SetCustomBehavior(sprite, onUpdate, onDestroy);
	return bullet;
}

void BulletPool::ReturnBullet(std::shared_ptr<Bullet> bullet)
{
	pool.push_back(bullet);
}