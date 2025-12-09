#pragma once
#include "Enemy.h"

class Army : public Enemy
{
private:
	int cooldown = 0;

	void Attack();

public:
	Army();
	Army(int x, int y, int health, float speed, int killScore);

	void Update() override;
};

