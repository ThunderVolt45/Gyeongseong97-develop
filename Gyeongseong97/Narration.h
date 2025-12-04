#pragma once
#include "GameObject.h"
#include "GameConstants.h"
#include "Enemy.h"

// Forward declaration
class BossState;
class BossStateAppeared;
class BossStateIdle;
class BossStateAttackShot;
class BossStateAttackDive;
class BossStateSpawnVanguard;
class BossStateSpawnCarmikaze;
class BossStateDeath;

enum class BossStateEnum
{
	Initialize, // 등장
	Idle, // 대기
	Shot, // 사격 패턴
	Dive, // 돌진 패턴
	Vanguard, // 소환 패턴 - 전위대
	Carmikaze, // 소환 패턴 - 자동차
	Dead // 시체로 결★정
};

class Narration : public Enemy
{
	// State classes need access to private members
	friend class BossState;
	friend class BossStateAppeared;
	friend class BossStateIdle;
	friend class BossStateAttackShot;
	friend class BossStateAttackDive;
	friend class BossStateSpawnVanguard;
	friend class BossStateSpawnCarmikaze;
	friend class BossStateDeath;

public:
	BossState* state; // Current state object
	BossStateEnum lastBossState;
	BossStateEnum bossState;

	// Made public for easier access by state classes (or rely on friends)
	// Keeping them public/accessible as requested by the refactoring style
	bool invincible = true;
	int defaultPosX = GAME_WIDTH / 2;
	int defaultPosY = 40;
	
	long long tick;
	long long lastStateChangeTick;
	long long internalTick;
	int internalCounter;

public:
	Narration(int x, int y, int health, float speed, int killScore);

	void TakeDamage(int damage) override;
	void Update() override;
	void OnCollision(GameObject& other) override;
	void ChangePattern();
	void Destroy() override;
};

