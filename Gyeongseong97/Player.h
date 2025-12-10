#pragma once
#include "GameObject.h"
#include "ImageLoader.h"
#include "Weapon.h"
#include "WeaponItem.h"

class Player : public GameObject
{
private:
	Sprite defaultSprite;
	Sprite deathSprite;
	int cooldown;

public:
	std::shared_ptr<Weapon> currentWeapon;
	std::shared_ptr<Weapon> defaultWeapon;
	std::shared_ptr<Weapon> grenadeWeapon;

	float maxHealth;
	float health;
	bool invincible;

public:
	Player();
	Player(int x, int y);
	~Player();

	void Reset();
	void Update() override;
	void OnCollision(GameObject& other) override;
	void TakeDamage(float damage);
	void Destroy();

	// 무기 시스템 메서드
	void Shoot();
	void Grenade();
	void EquipWeapon(std::shared_ptr<Weapon> newWeapon);
};