#pragma once
#include "Weapon.h"
#include "Bullet.h"

class WeaponRocket : public Weapon
{
public:
	WeaponRocket();
	void Shoot(Player* owner) override;
	static void RocketHomingBehavior(Bullet* b);
	static void ExplosionBehavior(Bullet* b);
	static void DelayedExplosion(Bullet* fuse);
};
