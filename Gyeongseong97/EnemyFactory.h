#pragma once
#include "StageManager.h"

struct EnemyInfo
{
	SpawnType type;
	int x;
	int y;
	int health = 1;
	float speed = 1.0f;
	int killScore = -1;
};

class EnemyFactory
{
public:
	static void CreateEnemy(EnemyInfo enemy);
};

