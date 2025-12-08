#pragma once
#include "Weapon.h"
#include "Bullet.h"

class WeaponRocket : public Weapon
{
public:
	WeaponRocket();
	void Shoot(Player* owner) override;
	static void RocketHomingBehavior(Bullet* b);
};
