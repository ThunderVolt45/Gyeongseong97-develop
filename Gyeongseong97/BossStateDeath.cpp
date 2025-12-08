#include "BossStateDeath.h"
#include "Narration.h"
#include "GameManager.h"
#include "ObjectPool.h"
#include "Explosion.h"
#include "Utility.h"
#include "Enums.h"

void BossStateDeath::Update(Narration& boss)
{
	GameManager& gameManager = GameManager::GetInstance();
	gameManager.player.invincible = true;

	boss.internalTick++;

	if (boss.internalTick == 1)
	{
		gameManager.DestroyAllEnemiesExcept(&boss);
	}

	// 폭★8 연출
	if (boss.internalTick % 15 == 0)
	{
		int x = Utility::GenerateRandomNumber(-40, 40);
		int y = Utility::GenerateRandomNumber(-30, 30);
		std::shared_ptr<Explosion> explosion = ObjectPool<Explosion>::GetInstance().Get(boss.GetCenterX() + x, boss.GetCenterY() + y);
		gameManager.CreateGameObject(explosion, TargetLayer::Background);
	}

	if (boss.internalTick >= 300)
	{
		boss.Destroy();
	}
}
