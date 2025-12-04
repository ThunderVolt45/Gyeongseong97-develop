#include "GameConstants.h"
#include "ExplosionPool.h"

ExplosionPool::ExplosionPool()
{

}

ExplosionPool& ExplosionPool::GetInstance()
{
	static ExplosionPool instance;
	return instance;
}

std::shared_ptr<Explosion> ExplosionPool::GetExplosion(int x, int y, int w, int h, int damage)
{
	if (pool.empty())
	{
		return std::make_shared<Explosion>(x, y, w, h, damage);
	}
	else
	{
		std::shared_ptr<Explosion> explosion = pool.back();
		pool.pop_back();

		explosion->Reset(x, y, w, h, damage);

		return explosion;
	}
}

void ExplosionPool::ReturnExplosion(std::shared_ptr<Explosion> explosion)
{
	pool.push_back(explosion);
}