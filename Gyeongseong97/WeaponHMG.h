#pragma once
#include "Weapon.h"

class WeaponHMG : public Weapon
{
public:
	WeaponHMG();
	void Shoot(Player* owner) override;
};