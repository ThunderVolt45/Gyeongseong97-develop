#pragma once
#include "Weapon.h"

class WeaponGrenade : public Weapon
{
public:
	WeaponGrenade();
	void Shoot(Player* owner) override;
};