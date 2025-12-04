#pragma once
#include "GameObject.h"
#include "Weapon.h"

class WeaponItem : public GameObject
{
public:
	float speed;
	WeaponType weaponType;

	WeaponItem(float x, float y, float speed, WeaponType type);
	void Update() override;
	void OnCollision(GameObject& other) override;
};