#pragma once
#include <vector>
#include <memory>
#include "Bullet.h"

class BulletPool
{
private:
	std::vector<std::shared_ptr<Bullet>> pool;
	BulletPool();

public:
	static BulletPool& GetInstance();
	std::shared_ptr<Bullet> GetBullet(int x, int y, float speedX, float speedY, bool isMine);
	void ReturnBullet(std::shared_ptr<Bullet> bullet);
};
