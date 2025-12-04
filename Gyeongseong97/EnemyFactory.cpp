#include "EnemyFactory.h"
#include "GameManager.h"
#include "GameConstants.h"
#include "Utility.h"
#include "Enemy.h"
#include "Vanguard.h"
#include "Carmikaze.h"
#include "Narration.h"

void EnemyFactory::CreateEnemy(EnemyInfo enemy)
{
	GameManager& gameManager = GameManager::GetInstance();

	// 적을 생성할 좌표 값을 구한다
	int spawnX = enemy.x;
	int spawnY = enemy.y;

	// x 좌표 값을 할당한다
	switch (spawnX)
	{
	case static_cast<int>(SpawnPosition::Random):
		spawnX = Utility::GenerateRandomNumber(10, GAME_WIDTH - 10);
		break;
	case static_cast<int>(SpawnPosition::PlayerPosition):
		spawnX = gameManager.player.GetCenterX();
		break;
	case static_cast<int>(SpawnPosition::Min):
		spawnX = 0;
		break;
	case static_cast<int>(SpawnPosition::Max):
		spawnX = GAME_WIDTH;
		break;
	}

	// y 좌표 값을 할당한다
	switch (spawnY)
	{
	case static_cast<int>(SpawnPosition::Random):
		spawnY = Utility::GenerateRandomNumber(10, GAME_HEIGHT - 10);
		break;
	case static_cast<int>(SpawnPosition::PlayerPosition):
		spawnY = gameManager.player.GetCenterY();
		break;
	case static_cast<int>(SpawnPosition::Min):
		spawnY = 0;
		break;
	case static_cast<int>(SpawnPosition::Max):
		spawnY = GAME_HEIGHT;
		break;
	}

	// 적을 생성한다
	switch (enemy.type)
	{
	case SpawnType::Vanguard:
	{
		auto vanguard = std::make_shared<Vanguard>(spawnX, spawnY, enemy.health, enemy.speed, 
			enemy.killScore == - 1 ? 500 : enemy.killScore);

		gameManager.CreateGameObject(vanguard);
		break;
	}
	case SpawnType::Carmikaze:
	{
		auto carmikaze = std::make_shared<Carmikaze>(spawnX, spawnY, enemy.health, enemy.speed, 
			enemy.killScore == -1 ? 1000 : enemy.killScore);

		gameManager.CreateGameObject(carmikaze);
		break;
	}
	case SpawnType::Narration:
	{
		auto narration = std::make_shared<Narration>(spawnX, spawnY, enemy.health, enemy.speed, 
			enemy.killScore == -1 ? 10000 : enemy.killScore);

		gameManager.CreateGameObject(narration);
		break;
	}
	case SpawnType::Instigated:
	{
		auto instigated = std::make_shared<Enemy>(spawnX, spawnY, enemy.health, enemy.speed, 
			enemy.killScore == -1 ? 0 : enemy.killScore);

		gameManager.CreateGameObject(instigated);
		break;
	}
	}
}