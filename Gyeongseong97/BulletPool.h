#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "Bullet.h"

class BulletPool
{
private:
	std::vector<std::shared_ptr<Bullet>> pool;

	BulletPool();

public:
	static BulletPool& GetInstance();
	std::shared_ptr<Bullet> GetBullet(int x, int y, float speedX, float speedY, bool isMine, int damage = 1, int lifeTick = -1);
	std::shared_ptr<Bullet> GetCustomBullet(
		int x, int y, float speedX, float speedY, bool isMine, int damage, int lifeTick,
		Sprite sprite, std::function<void(Bullet*)> onUpdate, std::function<void(Bullet*)> onDestroy);

	void ReturnBullet(std::shared_ptr<Bullet> bullet);
};