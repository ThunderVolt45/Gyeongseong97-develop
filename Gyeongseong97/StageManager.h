#pragma once
#include <vector>

enum class SpawnPosition : int
{
	Random = -100, // 무작위로 결정
	PlayerPosition = -101, // 플레이어의 x / y 값을 가져옴
	Min = -102, // 최소 값
	Max = -103 // 최대 값
};

enum class SpawnType : int
{
	Instigated,
	Vanguard,
	Carmikaze,
	Narration,
	ItemHMG = -1,
	ItemGranade = -2
};

struct SpawnData
{
	SpawnType type;
	int x = static_cast<int>(SpawnPosition::Random);
	int y = 0;
	int health = 1;
	float speed = 1.0f;
	float nextEnemyDelay; // 이 몬스터가 생성된 후 다음 몬스터 생성까지의 대기 시간
};

struct Wave
{
	std::vector<SpawnData> enemies; // 웨이브 동안 등장할 적의 목록
	float nextWaveDelay; // 이 웨이브가 끝나고 다음 웨이브까지의 대기 시간
};

class StageManager
{
private:
	std::vector<Wave> waves; // 웨이브 목록
	float tickTimeToFloat;
	long long tick;

	size_t currentWaveIndex = 0;
	size_t currentEnemyIndex = 0;
	float delayTimer = 0.0f;

	/// <summary>
	/// 모든 Wave의 모든 적을 처치하는데 성공했을 때 호출할 함수
	/// </summary>
	void GameClear();

public:
	/// <summary>
	/// StageManager를 초기화시키는 함수
	/// </summary>
	void Initialize();

	/// <summary>
	/// StageManager를 초기 상태로 되돌리는 함수
	/// </summary>
	void Reset();

	/// <summary>
	/// StageManager의 tick을 진행시키는 함수
	/// </summary>
	void Update();
};

