#pragma once
#include "Enemy.h"

class Vanguard : public Enemy
{
private:
	int direction = 1;

public:
	float health = 3;
	float speed = 1;
	int killScore = 500;

	Vanguard();
	Vanguard(int x, int y, int health, float speed, int killScore);

	void Update() override;
};

