#pragma once
#include "Enemy.h"

class Vanguard : public Enemy
{
private:
	int cooldown = 0;
	int stunTime = 0;

	void Tracking();
	void Attack();

public:
	Vanguard();
	Vanguard(int x, int y, int health, float speed, int killScore);

	void Update() override;
};

