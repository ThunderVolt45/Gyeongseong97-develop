#include <fstream>

#include "StageManager.h"
#include "GameManager.h"
#include "GameConstants.h"
#include "Utility.h"
#include "Enemy.h"
#include "Vanguard.h"
#include "Carmikaze.h"

#include "json.hpp"

using json = nlohmann::json;

void StageManager::CreateEnemy(SpawnData spawnData)
{
	// 적을 생성할 좌표 값을 구한다
	int spawnX = spawnData.x;
	int spawnY = spawnData.y;

	// x 좌표 값을 할당한다
	switch (spawnX)
	{
	case static_cast<int>(SpawnPosition::Random):
		spawnX = Utility::GenerateRandomNumber(0, GAME_WIDTH - 1);
		break;
	case static_cast<int>(SpawnPosition::PlayerPosition):
		spawnX = GameManager::GetInstance().player.GetCenterX();
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
		spawnY = Utility::GenerateRandomNumber(0, GAME_HEIGHT - 1);
		break;
	case static_cast<int>(SpawnPosition::PlayerPosition):
		spawnY = GameManager::GetInstance().player.GetCenterY();
		break;
	case static_cast<int>(SpawnPosition::Min):
		spawnY = 0;
		break;
	case static_cast<int>(SpawnPosition::Max):
		spawnY = GAME_HEIGHT;
		break;
	}

	// 적을 생성한다
	switch (spawnData.type)
	{
	case EnemyType::Vanguard:
	{
		auto vanguard = std::make_shared<Vanguard>(spawnX, spawnY, spawnData.health, spawnData.speed, 500);
		GameManager::GetInstance().CreateGameObject(vanguard);
		break;
	}
	case EnemyType::Carmikaze:
	{
		auto carmikaze = std::make_shared<Carmikaze>(spawnX, spawnY, spawnData.health, spawnData.speed, 1000);
		GameManager::GetInstance().CreateGameObject(carmikaze);
		break;
	}
	case EnemyType::Narration:
	{
		break;
	}
	default:
	{
		auto instigated = std::make_shared<Enemy>(spawnX, spawnY, spawnData.health, spawnData.speed, 0);
		GameManager::GetInstance().CreateGameObject(instigated);
		break;
	}
	}
}

void StageManager::Initialize()
{
	// 변수 초기화
	tick = 0;
	currentWaveIndex = 0;
	currentEnemyIndex = 0;
	delayTimer = 2.0f; // 첫 Wave는 2초 후 시작

	// tick 값 (마이크로초) 를 float 값 (초) 로 변환한다
	std::chrono::duration<float> floatSeconds = std::chrono::duration_cast<std::chrono::duration<float>>(TICK_TIME);
	tickTimeToFloat = floatSeconds.count();

	// json 파일을 연다
	std::ifstream file("stage_data.json");

	if (file.is_open())
	{
		// json 역직렬화
		json data = json::parse(file);

		// wave 값을 가져온다
		for (const auto& waveData : data["waves"])
		{
			Wave wave;
			wave.nextWaveDelay = waveData["nextWaveDelay"];

			// wave 내의 enemies 값을 가져온다
			for (const auto& enemyData : waveData["enemies"])
			{
				SpawnData spawn;
				spawn.type = static_cast<EnemyType>(enemyData["type"]);
				spawn.x = enemyData["x"];
				spawn.y = enemyData["y"];
				spawn.health = enemyData["health"];
				spawn.speed = enemyData["speed"];
				spawn.nextEnemyDelay = enemyData["nextEnemyDelay"];

				wave.enemies.push_back(spawn);
			}

			waves.push_back(wave);
		}
	}
	else
	{
		std::wcerr << "Error : stage_data.json을 불러오는데 실패했습니다." << std::endl;
	}

	// 파일을 모두 읽어들였다면 파일을 닫는다
	file.close();
}

void StageManager::Reset()
{
	tick = 0;
	currentWaveIndex = 0;
	currentEnemyIndex = 0;
	delayTimer = 2.0f;
}

void StageManager::Update()
{
	// 틱
	tick++;

	// 웨이브가 종료되면 중단
	if (waves.empty() || currentWaveIndex >= waves.size()) return;

	// 대기 시간 동안 대기한다 (?)
	if (delayTimer > 0)
	{
		delayTimer -= tickTimeToFloat;
		return;
	}

	// 현재 Wave 내의 모든 Enemy를 생성했다면 다음 Wave로 넘어간다
	const auto& currentWave = waves[currentWaveIndex];
	if (currentEnemyIndex >= currentWave.enemies.size())
	{
		delayTimer += currentWave.nextWaveDelay;
		currentWaveIndex++;
		currentEnemyIndex = 0;

		return;
	}

	// Wave 내의 Enemy를 생성한다.
	while (delayTimer <= 0) // delayTime이 0으로 설정된 적은 동시에 나오게 한다
	{
		if (currentEnemyIndex > currentWave.enemies.size() - 1)
			return;

		const auto& spawnData = currentWave.enemies[currentEnemyIndex];

		// 딜레이를 설정하고 현재 적 인덱스 값을 올린다.
		delayTimer += spawnData.nextEnemyDelay;
		currentEnemyIndex++;

		CreateEnemy(spawnData);
	}
}
