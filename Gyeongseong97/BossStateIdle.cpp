#include "BossStateIdle.h"
#include "Narration.h"
#include "Utility.h"
#include "ItemFactory.h"
#include <cmath>

// Include headers for state transitions
#include "BossStateAttackShot.h"
#include "BossStateAttackDive.h"
#include "BossStateAttackBomb.h"
#include "BossStateSpawnVanguard.h"
#include "BossStateSpawnCarmikaze.h"

void BossStateIdle::Update(Narration& boss)
{
	// 기본 위치로 이동
	float diffX = boss.defaultPosX - boss.GetCenterX();
	float diffY = boss.defaultPosY - boss.GetCenterY();
	float dist = std::sqrt(diffX * diffX + diffY * diffY);

	if (dist > boss.speed)
	{
		boss.x += (diffX / dist) * boss.speed;
		boss.y += (diffY / dist) * boss.speed;
	}
	else
	{
		boss.x += diffX;
		boss.y += diffY;
	}

	// 화면 중앙에 정렬될 때까지 기다린다
	if (dist > 1.0f)
	{
		return;
	}

	// 대기 타이머 동안 정지
	if (boss.internalTick > 0)
	{
		boss.internalTick--;
		return;
	}

	// 등장, 대기, 사망을 제외한 임의의 상태로 전이
	// 2: Shot, 3: Dive, 4: Bomb, 5: Vanguard, 6: Carmikaze
	int random;
	do
	{
		random = Utility::GenerateRandomNumber(2, 6);
	} while (random == static_cast<int>(boss.lastBossState));

	// 상태 전이
	boss.bossState = static_cast<BossStateEnum>(random); // Enum 업데이트

	switch (boss.bossState)
	{
	case BossStateEnum::Shot:
		boss.state = &BossState::shot;
		break;
	case BossStateEnum::Dive:
		boss.state = &BossState::dive;
		break;
	case BossStateEnum::Bomb:
		boss.state = &BossState::bomb;
		break;
	case BossStateEnum::Vanguard:
		boss.state = &BossState::vanguard;
		break;
	case BossStateEnum::Carmikaze:
		boss.state = &BossState::carmikaze;
		break;
	}
}
