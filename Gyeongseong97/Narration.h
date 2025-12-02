#pragma once
#include "GameObject.h"
#include "GameConstants.h"
#include "Enemy.h"

enum class BossState
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
private:
	bool invincible = true;
	int defaultPosX = GAME_WIDTH / 2;
	int defaultPosY = 40;

protected:
	void Appeared();
	void Idle();
	void AttackShot();
	void AttackDive();
	void SpawnVanguard();
	void SpawnCarmikaze();
	void EndPattern();
	void Destroy() override;

private:
	BossState lastBossState;
	BossState bossState;
	long long tick;
	long long lastStateChangeTick;
	long long waitTick;
	int internalCounter;

public:
	Narration(int x, int y, int health, float speed, int killScore);

	void Update() override;
	void OnCollision(GameObject& other) override;
};

