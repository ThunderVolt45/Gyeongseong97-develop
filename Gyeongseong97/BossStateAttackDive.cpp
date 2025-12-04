#include "BossStateAttackDive.h"
#include "Narration.h"
#include "GameManager.h"
#include "GameConstants.h"
#include <cmath>

void BossStateAttackDive::Update(Narration& boss)
{
	GameManager& gameManager = GameManager::GetInstance();
	boss.internalTick++;

	// 일정 틱 동안 플레이어의 x 축을 추적
	if (boss.internalTick < 120)
	{
		float diffX = gameManager.player.GetCenterX() - boss.GetCenterX();

		// 이동 속도 제한
		if (std::abs(diffX) > boss.speed * 2)
		{
			boss.x += (diffX > 0 ? boss.speed * 2 : -boss.speed * 2);
		}
		else
		{
			boss.x += diffX;
		}
	}
	// 빠르게 y축으로 이동
	else if (boss.y < GAME_HEIGHT - boss.sprite.sizeY / 2 - 1)
	{
		float diffY = GAME_HEIGHT - boss.sprite.sizeY / 2;

		// 이동 속도 제한
		if (std::abs(diffY) > boss.speed * 3)
		{
			boss.y += (diffY > 0 ? boss.speed * 3 : -boss.speed * 3);
		}
		else
		{
			boss.y += diffY;
		}
	}
	else
	{
		boss.ChangePattern();
	}
}
