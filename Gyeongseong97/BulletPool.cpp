#include "BulletPool.h"

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

void BulletPool::ReturnBullet(std::shared_ptr<Bullet> bullet)
{
	pool.push_back(bullet);
}
