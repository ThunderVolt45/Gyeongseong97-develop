#include <iostream> // For std::cout

#include "GameManager.h"
#include "AudioManager.h"
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
	lastBossState = BossState::Initialize;
	tick = 0;
	lastStateChangeTick = 0;
	internalTick = 0;

	// 기타 변수 초기화
	internalCounter = 0;
}

#pragma endregion

#pragma region Protected

void Narration::Appeared()
{
	internalTick++;

	if (internalTick < 120)
	{
		return;
	}

	// 기본 위치로 이동
	float diffX = defaultPosX - GetCenterX();
	float diffY = defaultPosY - GetCenterY();
	float dist = std::sqrt(diffX * diffX + diffY * diffY);

	if (dist > 0.2f)
	{
		x += (diffX / dist) * 0.2f;
		y += (diffY / dist) * 0.2f;
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

	// 정렬이 끝나면 보스전 시작
	invincible = false;
	EndPattern();
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
	if (internalTick > 0)
	{
		internalTick--;
		return;
	}

	// 등장, 대기, 사망을 제외한 임의의 상태로 전이
	int random;
	do
	{
		random = Utility::GenerateRandomNumber(2, 5);
	} while (random == static_cast<int>(lastBossState));

	bossState = static_cast<BossState>(random);
}

void Narration::AttackShot()
{
	GameManager& gameManager = GameManager::GetInstance();

	// 3점사 * 8회 = 24번 발사 후
	if (internalCounter >= 24)
	{
		EndPattern();
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
	if (internalTick <= 0)
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
			internalTick = 40;
		}
		else
		{
			// 연사 딜레이
			internalTick = 10;
		}
	}
	else
	{
		internalTick--;
	}
}

void Narration::AttackDive()
{
	GameManager& gameManager = GameManager::GetInstance();
	internalTick++;

	// 일정 틱 동안 플레이어의 x 축을 추적
	if (internalTick < 120)
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
		EndPattern();
	}

}

void Narration::SpawnVanguard()
{
	GameManager& gameManager = GameManager::GetInstance();

	// 전위대를 모두 소환했다면
	if (internalCounter >= 6)
	{
		// 스폰한 전위대가 거의 없어질 때까지 대기
		if (gameManager.GetEnemyCount() > 2)
		{
			return;
		}

		EndPattern();
		return;
	}

	// 전위대 소환 연출을 위한 차량 소환
	if (internalCounter <= 0)
	{
		internalCounter = 1;
		auto carmikaze = std::make_shared<Carmikaze>(GAME_WIDTH, 30, 100, 2.0f, 0);
		GameManager::GetInstance().CreateGameObject(carmikaze);

		internalTick += 24;
	}

	// 전위대 소환
	if (internalTick <= 0)
	{
		internalTick += 24;
		internalCounter++;

		int spawnX = GAME_WIDTH - 40 * (internalCounter - 1);
		auto vanguard = std::make_shared<Vanguard>(spawnX, 40, 3, 0.6f, 0);
		GameManager::GetInstance().CreateGameObject(vanguard);
	}
	else
	{
		internalTick--;
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

		EndPattern();
		return;
	}

	// 차량 소환
	if (internalTick <= 0)
	{
		// 30틱 마다 소환
		internalTick += 30;
		internalCounter++;

		int spawnX = internalCounter % 2 == 0 ? -30 : GAME_WIDTH + 30;
		int spawnY = Utility::GenerateRandomNumber(0, GAME_HEIGHT - 10); // 랜덤 생성
		auto carmikaze = std::make_shared<Carmikaze>(spawnX, spawnY, 4, 1.4f, 0);
		GameManager::GetInstance().CreateGameObject(carmikaze);
	}
	else
	{
		internalTick--;
	}
}

void Narration::Death()
{
	GameManager& gameManager = GameManager::GetInstance();
	gameManager.player.invincible = true;

	internalTick++;

	if (internalTick == 1)
	{
		gameManager.DestroyAllEnemiesExcept(this);
	}

	// 폭★8 연출
	if (internalTick % 15 == 0)
	{
		int x = Utility::GenerateRandomNumber(-40, 40);
		int y = Utility::GenerateRandomNumber(-30, 30);
		std::shared_ptr<Explosion> explosion(new Explosion(GetCenterX() + x, GetCenterY() + y));
		gameManager.CreateGameObject(explosion, false);
	}

	if (internalTick >= 300)
	{
		Destroy();
	}
}

void Narration::EndPattern()
{
	// 직전 상태 기록
	lastBossState = bossState;

	// 카운터 초기화
	internalCounter = 0;
	internalTick = 30;

	// 상태 전이
	bossState = BossState::Idle;
}

void Narration::Destroy()
{
	GameManager& gameManager = GameManager::GetInstance();

	std::shared_ptr<Explosion> explosion(new Explosion(GetCenterX(), GetCenterY(), 120, 90));
	gameManager.CreateGameObject(explosion, false);
	gameManager.DestroyGameObject(this);
	gameManager.score += killScore;

	gameManager.player.invincible = false;
}

#pragma endregion

#pragma region Public

void Narration::Update()
{
	tick++;
	ProcessHitEffect();

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
		Death();
		break;

	default:
		std::wcerr << L"나레이션 상태 에러 발생! 어째서 이런일이?" << std::endl;
		break;
	}
}
void Narration::OnCollision(GameObject& other)
{
	if (invincible) return;

	// 만약 Bullet과 충돌했다면
	Bullet* bullet = dynamic_cast<Bullet*>(&other);
	if (bullet)
	{
		// 플레이어가 쏜 총알에만 반응
		if (bullet->isPlayer)
		{
			health -= 1;
			hitEffectTick = 2;

			// 히트 당 기본 점수
			GameManager::GetInstance().score += SCORE_FOR_HIT;

			// 총알 파괴
			GameManager::GetInstance().DestroyGameObject(bullet);

			// 체력이 다 닳았으면 파괴
			if (health <= 0)
			{
				invincible = true;
				bossState = BossState::Dead;
				internalTick = 0;
			}
		}
		return;
	}

	// 만약 Player와 충돌했다면
	Player* player = dynamic_cast<Player*>(&other);
	if (player)
	{
		health -= 0.1f;
		hitEffectTick = 2;

		// 체력이 다 닳았으면 파괴
		if (health <= 0)
		{
			invincible = true;
			bossState = BossState::Dead;
			internalTick = 0;
		}

		return;
	}
}
#pragma endregion