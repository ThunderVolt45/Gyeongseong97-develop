#pragma once
#include <vector>
#include <memory>
#include "Bullet.h"
#include "BulletExplosive.h"

class BulletPool
{
private:
	std::vector<std::shared_ptr<Bullet>> pool;
	std::vector<std::shared_ptr<BulletExplosive>> explosivePool; // Explosive Bullet Pool 추가

	BulletPool();

public:
	static BulletPool& GetInstance();
	std::shared_ptr<Bullet> GetBullet(int x, int y, float speedX, float speedY, bool isMine);
	std::shared_ptr<Bullet> GetBullet(int x, int y, float speedX, float speedY, bool isMine, int damage);
	
	// BulletExplosive를 위한 GetBullet 오버로드
	std::shared_ptr<BulletExplosive> GetBullet(int x, int y, float speedX, float speedY, bool isMine, int damage, bool isExplosive);

	void ReturnBullet(std::shared_ptr<Bullet> bullet);
	void ReturnBullet(std::shared_ptr<BulletExplosive> bullet); // Explosive 총알 반환 추가
};
