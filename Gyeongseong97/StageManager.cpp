#include <fstream>

#include "StageManager.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "EnemyFactory.h"
#include "ItemFactory.h"
#include "Utility.h"
#include "Enemy.h"
#include "Vanguard.h"
#include "Carmikaze.h"
#include "Narration.h"
#include "GameConstants.h"

#include "json.hpp"

using json = nlohmann::json;

void StageManager::GameClear()
{
	GameManager::GetInstance().IsGameClear = true;

	AudioManager::GetInstance().StopAudio(BGM_BOSS.data());
	AudioManager::GetInstance().PlayAudio(BGM_WIN.data(), BGM_VOULME, false);
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
				spawn.type = static_cast<SpawnType>(enemyData["type"]);
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

	// 현재 Wave 내의 모든 Enemy를 생성했다면
	const auto& currentWave = waves[currentWaveIndex];
	if (currentEnemyIndex >= currentWave.enemies.size())
	{
		// 필드에 모든 적이 처치될 때까지 기다린다
		if (GameManager::GetInstance().IsEnemyAlive())
		{
			return;
		}

		// 모든 적이 처치되었고, 현재 웨이브가 마지막 웨이브라면 게임 승리
		if (currentWaveIndex >= waves.size() - 1)
		{
			GameClear();
			return;
		}

		// 아니면 다음 Wave로 넘어간다
		delayTimer += currentWave.nextWaveDelay;

		// 만약 다음 Wave가 마지막 Wave라면 추가 딜레이를 준다
		if (currentWaveIndex + 1 == waves.size() - 1)
		{
			AudioManager::GetInstance().FadeOutAudio(BGM_MAIN.data(), 3000);
			delayTimer += 3.0f;
		}

		currentWaveIndex++;
		currentEnemyIndex = 0;

		return;
	}

	// Wave 내의 Enemy를 생성한다.
	while (delayTimer <= 0) // delayTime이 0으로 설정된 적은 동시에 나오게 한다
	{
		if (currentEnemyIndex > currentWave.enemies.size() - 1)
			return;

		// 마지막 웨이브의 첫 번째 적이 스폰될 때 BGM 교체
		if (currentWaveIndex == waves.size() - 1 && currentEnemyIndex == 0)
		{
			AudioManager::GetInstance().PlayAudio(BGM_BOSS.data(), BGM_VOULME, true);
		}

		const auto& spawnData = currentWave.enemies[currentEnemyIndex];

		// 딜레이를 설정하고 현재 적 인덱스 값을 올린다.
		delayTimer += spawnData.nextEnemyDelay;
		currentEnemyIndex++;

		// 적, 아이템 생성
		if (static_cast<int>(spawnData.type) >= 0) // 적
		{
			EnemyInfo enemy;
			enemy.type = spawnData.type;
			enemy.x = spawnData.x;
			enemy.y = spawnData.y;
			enemy.health = spawnData.health;
			enemy.speed = spawnData.speed;
			enemy.killScore = -1;

			EnemyFactory::CreateEnemy(enemy);
		}
		else
		{
			ItemInfo item;
			item.type = spawnData.type;
			item.x = spawnData.x;
			item.y = spawnData.y;
			item.speed = spawnData.speed;

			ItemFactory::CreateItem(item);
		}
	}
}
