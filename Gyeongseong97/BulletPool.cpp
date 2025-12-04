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
	if (pool.empty())
	{
		return std::make_shared<Bullet>(x, y, speedX, speedY, isMine);
	}
	else
	{
		std::shared_ptr<Bullet> bullet = pool.back();
		pool.pop_back();

		bullet->Reset(x, y, speedX, speedY, isMine);

		return bullet;
	}
}

std::shared_ptr<Bullet> BulletPool::GetBullet(int x, int y, float speedX, float speedY, bool isMine, int damage)
{
	if (pool.empty())
	{
		return std::make_shared<Bullet>(x, y, speedX, speedY, isMine, damage);
	}
	else
	{
		std::shared_ptr<Bullet> bullet = pool.back();
		pool.pop_back();

		bullet->Reset(x, y, speedX, speedY, isMine, damage);

		return bullet;
	}
}

std::shared_ptr<BulletExplosive> BulletPool::GetBullet(int x, int y, float speedX, float speedY, bool isMine, int damage, bool isExplosive)
{
	if (explosivePool.empty())
	{
		return std::make_shared<BulletExplosive>(x, y, speedX, speedY, isMine, damage);
	}
	else
	{
		std::shared_ptr<BulletExplosive> bulletEx = explosivePool.back();
		explosivePool.pop_back();

		bulletEx->Reset(x, y, speedX, speedY, isMine, damage);

		return bulletEx;
	}
}

void BulletPool::ReturnBullet(std::shared_ptr<Bullet> bullet)
{
	pool.push_back(bullet);
}
