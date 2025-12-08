#include "BossStateAttackShot.h"
#include "Narration.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "ObjectPool.h"
#include "Enums.h"

#include <cmath>

void BossStateAttackShot::Update(Narration& boss)
{
	GameManager& gameManager = GameManager::GetInstance();

	// 3점사 * 8회 = 24번 발사 후
	if (boss.internalCounter >= 24)
	{
		boss.ChangePattern();
		return;
	}

	// 플레이어의 x축을 추적
	float diffX = gameManager.player.GetCenterX() - boss.GetCenterX();

	// 이동 속도 제한
	if (std::abs(diffX) > boss.speed * 1.2f)
	{
		boss.x += (diffX > 0 ? boss.speed * 1.2f : -boss.speed * 1.2f);
	}
	else
	{
		boss.x += diffX;
	}

	// 사격 개시
	if (boss.internalTick <= 0)
	{
		// 총알 발사!!!
		std::shared_ptr<Bullet> bullet1 = ObjectPool<Bullet>::GetInstance().Get(boss.GetCenterX() - 15, boss.GetCenterY(), 0.0f, -2.0f, false);
		std::shared_ptr<Bullet> bullet2 = ObjectPool<Bullet>::GetInstance().Get(boss.GetCenterX(), boss.GetCenterY(), 0.0f, -2.0f, false);
		std::shared_ptr<Bullet> bullet3 = ObjectPool<Bullet>::GetInstance().Get(boss.GetCenterX() + 15, boss.GetCenterY(), 0.0f, -2.0f, false);
		gameManager.CreateGameObject(bullet1, TargetLayer::Foreground);
		gameManager.CreateGameObject(bullet2, TargetLayer::Foreground);
		gameManager.CreateGameObject(bullet3, TargetLayer::Foreground);

		// 사격음 출력
		AudioManager::GetInstance().PlayAudio(SFX_GUNFIRE.data(), 0.3f);

		boss.internalCounter++;

		// 3발 쏠 때마다(3점사 완료 시) 긴 딜레이
		if (boss.internalCounter % 3 == 0)
		{
			boss.internalTick = 40;
		}
		else
		{
			// 연사 딜레이
			boss.internalTick = 10;
		}
	}
	else
	{
		boss.internalTick--;
	}
}
