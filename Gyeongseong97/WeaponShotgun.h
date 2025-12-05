#pragma once
#include "Weapon.h"

class WeaponShotgun : public Weapon
{
public:
	WeaponShotgun();
	void Shoot(Player* owner) override;
};