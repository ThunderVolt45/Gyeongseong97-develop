#pragma once
#include "GameObject.h"
#include "ImageLoader.h"
#include "Weapon.h"
#include "WeaponItem.h"

class Player : public GameObject
{
public:
	std::shared_ptr<Weapon> currentWeapon;
	std::shared_ptr<Weapon> defaultWeapon;
	std::shared_ptr<Weapon> grenadeWeapon;

	Sprite defaultSprite;
	Sprite deathSprite;
	
	float maxHealth;
	float health;
	int cooldown;
	bool invincible;

	void Destroy();

public:
	Player();
	Player(int x, int y);
	~Player(); // 소멸자 추가

	void Reset();
	void Update() override;
	void OnCollision(GameObject& other) override;
	void TakeDamage(float damage);

	// 무기 시스템 메서드
	void Shoot();
	void Grenade();
	void EquipWeapon(std::shared_ptr<Weapon> newWeapon);
};