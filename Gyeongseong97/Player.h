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

	// 새로운 무기 시스템 메서드
	void Shoot();
	void EquipWeapon(std::shared_ptr<Weapon> newWeapon);
};