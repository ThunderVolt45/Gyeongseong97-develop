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

std::shared_ptr<Bullet> BulletPool::GetBullet(int x, int y, float speedX, float speedY, bool isMine)
{
	return GetBullet(x, y, speedX, speedY, isMine, 1, false);
}

std::shared_ptr<Bullet> BulletPool::GetBullet(int x, int y, float speedX, float speedY, bool isMine, int damage)
{
	return GetBullet(x, y, speedX, speedY, isMine, damage, false);
}

std::shared_ptr<Bullet> BulletPool::GetBullet(int x, int y, float speedX, float speedY, bool isMine, int damage, bool isExplosive)
{
	if (pool.empty())
	{
		auto bullet = std::make_shared<Bullet>();
		bullet->Reset(x, y, speedX, speedY, isMine, damage, isExplosive);
		return bullet;
	}
	else
	{
		std::shared_ptr<Bullet> bullet = pool.back();
		pool.pop_back();

		bullet->Reset(x, y, speedX, speedY, isMine, damage, isExplosive);
		return bullet;
	}
}

void BulletPool::ReturnBullet(std::shared_ptr<Bullet> bullet)
{
	pool.push_back(bullet);
}