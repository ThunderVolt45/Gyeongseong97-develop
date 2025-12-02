#include <iostream> // For std::cout
#include <cmath>

#include "GameManager.h"
#include "Utility.h"
#include "GameConstants.h"
#include "Narration.h"
#include "Carmikaze.h"
#include "Vanguard.h"
#include "Explosion.h"

#pragma region Constructer & Destroyer

Narration::Narration(int x, int y, int health, float speed, int killScore)
{
	this->health = health;
	this->speed = speed;
	this->killScore = killScore;

	originalSprite = ImageLoader::CreateSpriteFromImage(L"enemy_narration.png", 80, 60);
	sprite = originalSprite;

	hitSprite = ImageLoader::CreateHitSprite(sprite);

	this->x = (float)(x - (sprite.sizeX / 2));
	this->y = (float)(y - (sprite.sizeY / 2));

	// 보스 상태 및 타이머 초기화
	bossState = BossState::Initialize;
	tick = 0;
	lastStateChangeTick = 0;
	waitTick = 0;

	// 기타 변수 초기화
	internalCounter = 0;
}

#pragma endregion

#pragma region Protected

void Narration::Appeared()
{
	// TODO: 등장 연출
	bossState = BossState::Idle;
}

void Narration::Idle()
{
	// 화면 중앙 상단으로 이동
	float targetX = GAME_WIDTH / 2;
	float targetY = 40;

	float diffX = targetX - GetCenterX();
	float diffY = targetY - GetCenterY();
	float dist = std::sqrt(diffX * diffX + diffY * diffY);

	if (dist > speed)
	{
		x += (diffX / dist) * speed;
		y += (diffY / dist) * speed;
	}
	else
	{
		x += diffX;
		y += diffY;
	}

	// 대기 타이머 동안 정지
	if (waitTick > 0)
	{
		waitTick--;
		return;
	}

	// 등장, 대기, 사망을 제외한 임의의 상태로 전이
	int random = Utility::GenerateRandomNumber(2, 5);
	bossState = static_cast<BossState>(random);
}

void Narration::AttackShot()
{
	GameManager& gameManager = GameManager::GetInstance();

	// 8번 사격했다면 중단
	if (internalCounter >= 8 * 3)
	{
		// 카운터 초기화
		waitTick = 60;
		internalCounter = 0;

		// 상태 전이
		bossState = BossState::Idle;

		return;
	}

	// 플레이어의 x 축을 추적
	float diffX = gameManager.player.GetCenterX() - GetCenterX();
	x += diffX > speed ? diffX  * speed : diffX;

	// 사격 개시
	if (waitTick <= 0)
	{
		waitTick += 30;

		// 총알 발사!!!
		std::shared_ptr<Bullet> bullet1(new Bullet(GetCenterX() - 15, GetCenterY(), 0.0f, -2.0f, false));
		std::shared_ptr<Bullet> bullet2(new Bullet(GetCenterX(), GetCenterY(), 0.0f, -2.0f, false));
		std::shared_ptr<Bullet> bullet3(new Bullet(GetCenterX() + 15, GetCenterY(), 0.0f, -2.0f, false));
		GameManager::GetInstance().CreateGameObject(bullet1);
		GameManager::GetInstance().CreateGameObject(bullet2);
		GameManager::GetInstance().CreateGameObject(bullet3);
	}

	waitTick--;
}

void Narration::AttackDive()
{
	// TODO: 패턴 만들기
	bossState = BossState::Idle;

	// 일정 틱 동안 플레이어의 x 축을 추적

	// 빠르게 y축으로 이동
}

void Narration::SpawnVanguard()
{
	GameManager& gameManager = GameManager::GetInstance();

	// 전위대를 소환했다면
	if (internalCounter >= 6)
	{
		// 스폰한 전위대가 모두 없어질 때까지 대기
		if (gameManager.GetEnemyCount() > 1)
		{
			return;
		}

		// 카운터 초기화
		internalCounter = 0;
		waitTick = 60;

		// 상태 전이
		bossState = BossState::Idle;

		return;
	}

	// 전위대 소환 연출을 위한 차량 소환
	if (internalCounter <= 0)
	{
		internalCounter = 1;
		auto carmikaze = std::make_shared<Carmikaze>(GAME_WIDTH, 10, 100, 2.0f, 0);
		GameManager::GetInstance().CreateGameObject(carmikaze);
	}

	// 전위대 소환
	if (waitTick <= 0)
	{
		waitTick += 24;
		internalCounter++;

		int spawnX = GAME_WIDTH - 40 * (internalCounter - 1);
		auto vanguard = std::make_shared<Vanguard>(spawnX, 15, 3, 0.6f, 0);
		GameManager::GetInstance().CreateGameObject(vanguard);
	}
	else
	{
		waitTick--;
	}
	
}

void Narration::SpawnCarmikaze()
{
	GameManager& gameManager = GameManager::GetInstance();

	// 차량을 모두 소환했다면
	if (internalCounter >= 8)
	{
		// 스폰한 차량이 모두 없어질 때까지 대기
		if (gameManager.GetEnemyCount() > 1)
		{
			return;
		}

		// 카운터 초기화
		internalCounter = 0;
		waitTick = 60;

		// 상태 전이
		bossState = BossState::Idle;

		return;
	}

	// 차량 소환
	if (waitTick <= 0)
	{
		// 30틱 마다 소환
		waitTick += 30;
		internalCounter++;

		int spawnX = internalCounter % 2 == 0 ? 0 : GAME_WIDTH;
		int spawnY = Utility::GenerateRandomNumber(0, GAME_HEIGHT - 1); // 랜덤 생성
		auto carmikaze = std::make_shared<Carmikaze>(spawnX, spawnY, 5, 1.4f, 0);
		GameManager::GetInstance().CreateGameObject(carmikaze);
	}
	else
	{
		waitTick--;
	}
}

void Narration::Destroy()
{
	GameManager& gameManager = GameManager::GetInstance();

	std::shared_ptr<Explosion> explosion(new Explosion(GetCenterX(), GetCenterY(), 80, 60));
	gameManager.CreateGameObject(explosion, false);
	gameManager.DestroyGameObject(this);
	gameManager.score += killScore;

	// TODO: 나레이션 격파 연출

	// TODO: 게임 승리 처리
}

#pragma endregion

#pragma region Public

void Narration::Update()
{
	tick++;
	ProcessHitEffect();
	
	// 마지막 상태 전이 이후로 진행된 tick 계산
	long long tickSinceLastStateChange = tick - lastStateChangeTick;

	switch (bossState)
	{
	case BossState::Initialize:
		Appeared();
		break;

	case BossState::Idle:
		Idle();
		break;

	case BossState::Shot:
		AttackShot();
		break;

	case BossState::Dive:
		AttackDive();
		break;

	case BossState::Vanguard:
		SpawnVanguard();
		break;

	case BossState::Carmikaze:
		SpawnCarmikaze();
		break;

	case BossState::Dead:
		// 시체로 결★정된 이후로는 FSM 정지
		break;

	default:
		std::wcerr << L"나레이션 상태 에러 발생! 어째서 이런일이?" << std::endl;
		break;
	}
}
void Narration::OnCollision(GameObject& other)
{
	Enemy::OnCollision(other);
	
	// 체력이 모두 고갈되면 시체로 결★정
	if (health <= 0)
	{
		bossState = BossState::Dead;
	}
}
#pragma endregion