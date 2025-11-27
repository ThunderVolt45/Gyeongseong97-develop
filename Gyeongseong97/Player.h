#pragma once
#include "GameObject.h"

class Player : public GameObject
{
public:
	float maxHealth;
	float health;
	int cooldown;

	Player();
	Player(int x, int y, int w, int h, std::wstring spriteName);

	void Reset();
	void Update() override;
	void OnCollision(GameObject& other) override;
};

