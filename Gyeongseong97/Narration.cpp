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
	// 기본 위치로 이동
	float diffX = defaultPosX - GetCenterX();
	float diffY = defaultPosY - GetCenterY();
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

	// 화면 중앙에 정렬될 때까지 기다린다
	if (dist > 1.0f)
	{
		return;
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

	// 3점사 * 8회 = 24번 발사 후
	if (internalCounter >= 24)
	{
		// 카운터 초기화
		waitTick = 30;
		internalCounter = 0;

		// 상태 전이
		bossState = BossState::Idle;

		return;
	}

	// 플레이어의 x축을 추적
	float diffX = gameManager.player.GetCenterX() - GetCenterX();
	
	// 이동 속도 제한
	if (std::abs(diffX) > speed * 1.2f)
	{
		x += (diffX > 0 ? speed * 1.2f : -speed * 1.2f);
	}
	else
	{
		x += diffX;
	}

	// 사격 개시
	if (waitTick <= 0)
	{
		// 총알 발사!!!
		std::shared_ptr<Bullet> bullet1(new Bullet(GetCenterX() - 15, GetCenterY(), 0.0f, -2.0f, false));
		std::shared_ptr<Bullet> bullet2(new Bullet(GetCenterX(), GetCenterY(), 0.0f, -2.0f, false));
		std::shared_ptr<Bullet> bullet3(new Bullet(GetCenterX() + 15, GetCenterY(), 0.0f, -2.0f, false));
		gameManager.CreateGameObject(bullet1);
		gameManager.CreateGameObject(bullet2);
		gameManager.CreateGameObject(bullet3);

		internalCounter++;

		// 3발 쏠 때마다(3점사 완료 시) 긴 딜레이
		if (internalCounter % 3 == 0)
		{
			waitTick = 40;
		}
		else
		{
			// 연사 딜레이
			waitTick = 10;
		}
	}
	else
	{
		waitTick--;
	}
}

void Narration::AttackDive()
{
	GameManager& gameManager = GameManager::GetInstance();
	waitTick++;

	// 일정 틱 동안 플레이어의 x 축을 추적
	if (waitTick < 120)
	{
		float diffX = gameManager.player.GetCenterX() - GetCenterX();

		// 이동 속도 제한
		if (std::abs(diffX) > speed * 2)
		{
			x += (diffX > 0 ? speed * 2 : -speed * 2);
		}
		else
		{
			x += diffX;
		}
	}
	// 빠르게 y축으로 이동
	else if (y < GAME_HEIGHT - sprite.sizeY / 2 - 1)
	{
		float diffY = GAME_HEIGHT - sprite.sizeY / 2;
		
		// 이동 속도 제한
		if (std::abs(diffY) > speed * 3)
		{
			y += (diffY > 0 ? speed * 3 : -speed * 3);
		}
		else
		{
			y += diffY;
		}
	}
	else
	{
		// 카운터 초기화
		waitTick = 60;
		internalCounter = 0;

		// 상태 전이
		bossState = BossState::Idle;
	}

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
		waitTick = 30;

		// 상태 전이
		bossState = BossState::Idle;

		return;
	}

	// 전위대 소환 연출을 위한 차량 소환
	if (internalCounter <= 0)
	{
		internalCounter = 1;
		auto carmikaze = std::make_shared<Carmikaze>(GAME_WIDTH, 35, 100, 2.0f, 0);
		GameManager::GetInstance().CreateGameObject(carmikaze);
	}

	// 전위대 소환
	if (waitTick <= 0)
	{
		waitTick += 24;
		internalCounter++;

		int spawnX = GAME_WIDTH - 40 * (internalCounter - 1);
		auto vanguard = std::make_shared<Vanguard>(spawnX, 40, 3, 0.6f, 0);
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
		waitTick = 30;

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