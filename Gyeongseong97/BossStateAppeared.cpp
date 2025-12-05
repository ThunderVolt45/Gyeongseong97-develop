#include "BossStateAppeared.h"
#include "Narration.h"
#include "GameConstants.h"
#include <cmath>

void BossStateAppeared::Update(Narration& boss)
{
	boss.internalTick++;

	if (boss.internalTick < 120)
	{
		return;
	}

	// 기본 위치로 이동
	float diffX = boss.defaultPosX - boss.GetCenterX();
	float diffY = boss.defaultPosY - boss.GetCenterY();
	float dist = std::sqrt(diffX * diffX + diffY * diffY);

	if (dist > 0.2f)
	{
		boss.x += (diffX / dist) * 0.2f;
		boss.y += (diffY / dist) * 0.2f;
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

	// 정렬이 끝나면 보스전 시작
	boss.invincible = false;
	boss.ChangePattern();
}
