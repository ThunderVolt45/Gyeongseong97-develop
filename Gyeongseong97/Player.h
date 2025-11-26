#pragma once
#include "GameObject.h"

class Player : public GameObject
{
private:
	float maxHealth;

public:
	float health;
	long long tick;
	int cooldown;

	Player();
	Player(int x, int y, int w, int h, std::wstring spriteName);

	void Reset();
	void Update() override;
};

