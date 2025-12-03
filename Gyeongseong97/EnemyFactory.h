#pragma once
#include "GameManager.h"
#include "GameConstants.h"
#include "StageManager.h"

struct EnemyInfo
{
	EnemyType type;
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

