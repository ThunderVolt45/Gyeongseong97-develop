#include "StageManager.h"
#include "GameManager.h"
#include "GameConstants.h"
#include "Enemy.h"

void StageManager::Initialize()
{

}

void StageManager::Reset()
{
	tick = 0;
}

void StageManager::Update()
{
	tick++;

	if (tick % 30 == 0)
	{
		int randomX = std::rand() % (GAME_WIDTH - 4) + 2;
		auto enemy = std::make_shared<Enemy>(randomX, 0, 1, 0.5f, 100);
		GameManager::GetInstance().CreateGameObject(enemy);
	}
}
