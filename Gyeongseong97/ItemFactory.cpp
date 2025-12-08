#include "ItemFactory.h"
#include "GameManager.h"
#include "GameConstants.h"
#include "Enums.h"
#include "Utility.h"
#include "Weapon.h"

void ItemFactory::CreateItem(ItemInfo item)
{
	GameManager& gameManager = GameManager::GetInstance();

	// 아이템을 생성할 좌표 값을 구한다
	int spawnX = item.x;
	int spawnY = item.y;

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

	// 아이템을 생성한다
	switch (item.type)
	{
	case SpawnType::ItemHMG:
	{
		auto hmg = std::make_shared<WeaponItem>
			(spawnX, spawnY, item.speed, WeaponType::HMG);
		gameManager.CreateGameObject(hmg);
		break;
	}
	case SpawnType::ItemShotgun:
	{
		auto shotgun = std::make_shared<WeaponItem>
			(spawnX, spawnY, item.speed, WeaponType::Shotgun);
		gameManager.CreateGameObject(shotgun);
		break;
	}
	case SpawnType::ItemGranade:
	{
		auto granade = std::make_shared<WeaponItem>
			(spawnX, spawnY, item.speed, WeaponType::Grenade);
		gameManager.CreateGameObject(granade);
		break;
	}
	case SpawnType::ItemRocket:
	{
		auto rocket = std::make_shared<WeaponItem>
			(spawnX, spawnY, item.speed, WeaponType::Rocket);
		gameManager.CreateGameObject(rocket);
		break;
	}
	}
}
