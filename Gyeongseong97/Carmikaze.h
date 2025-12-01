#pragma once
#include "Enemy.h"

class Carmikaze : public Enemy
{
private:
	int direction = 1;

public:
	Carmikaze();
	Carmikaze(int x, int y, int health, float speed, int killScore);

	void Update() override;
};

