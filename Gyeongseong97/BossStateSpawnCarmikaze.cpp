#include "BossStateSpawnCarmikaze.h"
#include "Narration.h"
#include "GameManager.h"
#include "GameConstants.h"
#include "EnemyFactory.h"
#include "Utility.h"

void BossStateSpawnCarmikaze::Update(Narration& boss)
{
	GameManager& gameManager = GameManager::GetInstance();

	// 차량을 모두 소환했다면
	if (boss.internalCounter >= 8)
	{
		// 스폰한 차량이 모두 없어질 때까지 대기
		if (gameManager.GetEnemyCount() > 1)
		{
			return;
		}

		boss.ChangePattern();
		return;
	}

	// 차량 소환
	if (boss.internalTick <= 0)
	{
		// 30틱 마다 소환
		boss.internalTick += 30;
		boss.internalCounter++;

		int spawnX = boss.internalCounter % 2 == 0 ? -30 : GAME_WIDTH + 30;
		int spawnY = Utility::GenerateRandomNumber(10, GAME_HEIGHT - 10); // 랜덤 생성
		EnemyInfo enemy(SpawnType::Carmikaze, spawnX, spawnY, 5, 1.4f, 0);
		EnemyFactory::CreateEnemy(enemy);
	}
	else
	{
		boss.internalTick--;
	}
}
