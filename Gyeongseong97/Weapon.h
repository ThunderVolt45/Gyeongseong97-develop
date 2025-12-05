#pragma once
#include <functional>
#include "GameObject.h"

class Player; // Forward declaration

enum class WeaponType
{
	Default,
	HMG,
	Shotgun,
	Grenade
};

class Weapon
{
public:
	WeaponType type;
	int damage;
	int maxBullet;
	int remainBullet;
	bool isInfinite;
	
	int cooldownTick;

	Weapon(WeaponType type, int damage, int maxBullet, int cooldownTick, bool isInfinite)
		: type(type), damage(damage), maxBullet(maxBullet), cooldownTick(cooldownTick), 
			remainBullet(maxBullet), isInfinite(isInfinite) {}

	virtual ~Weapon() = default;

	// 순수 가상 함수: 각 무기마다 발사 로직이 다름
	virtual void Shoot(Player* owner) = 0;

	void AddAmmo(int bullet);

	bool IsEmpty() const
	{
		return !isInfinite && remainBullet <= 0;
	}
};

class WeaponDefault : public Weapon
{
public:
	WeaponDefault();
	void Shoot(Player* owner) override;
};