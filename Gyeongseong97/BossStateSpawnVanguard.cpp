#include "BossStateSpawnVanguard.h"
#include "Narration.h"
#include "GameManager.h"
#include "GameConstants.h"
#include "EnemyFactory.h"

void BossStateSpawnVanguard::Update(Narration& boss)
{
	GameManager& gameManager = GameManager::GetInstance();

	// 전위대를 모두 소환했다면
	if (boss.internalCounter >= 6)
	{
		// 스폰한 전위대가 거의 없어질 때까지 대기
		if (gameManager.GetEnemyCount() > 2)
		{
			return;
		}

		boss.ChangePattern();
		return;
	}

	// 전위대 소환 연출을 위한 차량 소환
	if (boss.internalCounter <= 0)
	{
		boss.internalCounter = 1;

		EnemyInfo enemy;
		enemy.type = SpawnType::Carmikaze;
		enemy.x = GAME_WIDTH;
		enemy.y = 30;
		enemy.health = 100;
		enemy.speed = 2.0f;
		enemy.killScore = 0;

		EnemyFactory::CreateEnemy(enemy);

		boss.internalTick += 24;
	}

	// 전위대 소환
	if (boss.internalTick <= 0)
	{
		boss.internalTick += 24;
		boss.internalCounter++;

		EnemyInfo enemy;
		enemy.type = SpawnType::Vanguard;
		enemy.x = GAME_WIDTH - 40 * (boss.internalCounter - 1);
		enemy.y = 40;
		enemy.health = 3;
		enemy.speed = 0.6f;
		enemy.killScore = 0;

		EnemyFactory::CreateEnemy(enemy);
	}
	else
	{
		boss.internalTick--;
	}
}
