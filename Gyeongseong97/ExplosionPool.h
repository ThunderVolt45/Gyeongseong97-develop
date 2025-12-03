#pragma once
#include <vector>

#include "Explosion.h"

class ExplosionPool
{
private:
	std::vector<std::shared_ptr<Explosion>> pool;
	ExplosionPool();

public:
	static ExplosionPool& GetInstance();
	std::shared_ptr<Explosion> GetExplosion(int x, int y, int w = EXPLOSION_DEFAULT_SIZE_X, int h = EXPLOSION_DEFAULT_SIZE_Y);
	void ReturnExplosion(std::shared_ptr<Explosion> explosion);
};

