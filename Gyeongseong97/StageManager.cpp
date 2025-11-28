#include <fstream>

#include "StageManager.h"
#include "GameManager.h"
#include "GameConstants.h"
#include "Utility.h"
#include "Enemy.h"
#include "json.hpp"

using json = nlohmann::json;

void StageManager::CreateEnemy(SpawnData spawnData)
{
	// 적을 생성할 좌표 값을 구한다
	int spawnX = spawnData.x;
	int spawnY = spawnData.y;

	// 무작위 값이라면 랜덤 값을 넣어준다
	if (spawnX == static_cast<int>(SpawnPosition::Random))
	{
		spawnX = Utility::GenerateRandomNumber(0, GAME_WIDTH - 1);
	}

	if (spawnY == static_cast<int>(SpawnPosition::Random))
	{
		spawnY = Utility::GenerateRandomNumber(0, GAME_HEIGHT - 1);
	}

	// 플레이어의 좌표 값이라면 해당 값을 가져와서 넣어준다
	if (spawnX == static_cast<int>(SpawnPosition::PlayerPosition))
	{
		spawnX = GameManager::GetInstance().player.GetCenterX();
	}

	if (spawnY == static_cast<int>(SpawnPosition::PlayerPosition))
	{
		spawnY = GameManager::GetInstance().player.GetCenterY();
	}

	// 적을 생성한다
	auto enemy = std::make_shared<Enemy>(spawnX, spawnY, 1.5f, 0.5f, 100);
	GameManager::GetInstance().CreateGameObject(enemy);
}

void StageManager::Initialize()
{
	// 변수 초기화
	tick = 0;
	currentWaveIndex = 0;
	currentEnemyIndex = 0;
	delayTimer = 2.0f; // 첫 Wave는 1초 후 시작

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
