#pragma once
#include <vector>

enum class SpawnPosition : int
{
	Random = -100, // 무작위로 결정
	PlayerPosition = -101 // 플레이어의 x / y 값을 가져옴
};

enum class EnemyType : int
{
	Instigated,
	Vanguard,
	Carmikaze,
	Narration
};

struct SpawnData
{
	EnemyType type;
	int x = static_cast<int>(SpawnPosition::Random);
	int y = 0;
	float nextEnemyDelay; // 이 몬스터가 생성된 후 다음 몬스터 생성까지의 대기 시간
};

struct Wave
{
	std::vector<SpawnData> enemies; // 웨이브 동안 등장할 적의 목록
	float nextWaveDelay; // 이 웨이브가 끝나고 다음 웨이브까지의 대기 시간
};

class StageManager
{
public:
	void Initialize();
	void Update();
};

