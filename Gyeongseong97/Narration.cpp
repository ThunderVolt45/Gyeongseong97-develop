#include "GameManager.h"
#include "ItemFactory.h"
#include "Utility.h"
#include "GameConstants.h"
#include "Narration.h"
#include "Carmikaze.h"
#include "Vanguard.h"
#include "BulletPool.h"
#include "ExplosionPool.h"

// 상태 패턴 헤더
#include "BossState.h"
#include "BossStateAppeared.h"
#include "BossStateAttackBomb.h"
#include "BossStateAttackDive.h"
#include "BossStateAttackShot.h"
#include "BossStateDeath.h"
#include "BossStateIdle.h"
#include "BossStateSpawnCarmikaze.h"
#include "BossStateSpawnVanguard.h"

#pragma region Constructer & Destroyer

Narration::Narration(int x, int y, int health, float speed, int killScore)
{
	this->health = health;
	this->speed = speed;
	this->killScore = killScore;

	originalSprite = ImageLoader::CreateSpriteFromImage(SPR_NARRATION.data(), 80, 60);
	sprite = originalSprite;

	hitSprite = ImageLoader::CreateHitSprite(sprite);

	this->x = (float)(x - (sprite.sizeX / 2));
	this->y = (float)(y - (sprite.sizeY / 2));

	// 보스 상태 및 타이머 초기화
	state = &BossState::appeared; // 초기 상태 설정
	bossState = BossStateEnum::Initialize;
	lastBossState = BossStateEnum::Initialize;
	internalTick = 0;

	// 기타 변수 초기화
	internalCounter = 0;
}

#pragma endregion

#pragma region Public

void Narration::TakeDamage(float damage)
{
	if (invincible) return;

	health -= damage;
	hitEffectTick = 2;

	// 체력이 다 닳았으면 파괴 (상태 전이)
	if (health <= 0)
	{
		invincible = true;
		bossState = BossStateEnum::Dead;
		state = &BossState::death;
		internalTick = 0;
	}
}

void Narration::ChangePattern()
{
	// 직전 상태 기록
	lastBossState = bossState;

	// 카운터 초기화
	internalCounter = 0;
	internalTick = 30;

	// 낮은 확률 (20% 정도) 로 아이템 생성
	int random = Utility::GenerateRandomNumber(0, 99);
	if (random < 20)
	{
		ItemInfo item;
		item.x = Utility::GenerateRandomNumber(10, GAME_WIDTH - 10);
		item.y = 0;
		item.speed = 0.5f;

		switch (Utility::GenerateRandomNumber(0, 2))
		{
		case 0:
			item.type = SpawnType::ItemHMG;
			break;
		case 1:
			item.type = SpawnType::ItemShotgun;
			break;
		case 2:
			item.type = SpawnType::ItemGranade;
			break;
		}

		ItemFactory::CreateItem(item);
	}

	// 상태 전이 -> Idle로 복귀
	bossState = BossStateEnum::Idle;
	state = &BossState::idle;
}

void Narration::Update()
{
	ProcessHitEffect();
	
	// 상태 패턴 위임
	state->Update(*this);
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
			// 데미지 처리
			TakeDamage(bullet->GetDamage());

			// 히트 당 기본 점수
			GameManager::GetInstance().score += SCORE_FOR_HIT;

			// 총알 파괴
			bullet->Destroy();
		}
		return;
	}

	// 만약 Player와 충돌했다면
	Player* player = dynamic_cast<Player*>(&other);
	if (player)
	{
		TakeDamage(COLLISION_DAMAGE);

		// 보스 몹이니 특별히 플레이어에게 추가 데미지 부여
		player->TakeDamage(COLLISION_DAMAGE);
	}
}

void Narration::Destroy()
{
	GameManager& gameManager = GameManager::GetInstance();

	std::shared_ptr<Explosion> explosion = ExplosionPool::GetInstance().GetExplosion(GetCenterX(), GetCenterY(), 120, 90);
	gameManager.CreateGameObject(explosion, false);
	gameManager.DestroyGameObject(this);
	gameManager.score += killScore;

	gameManager.player.invincible = false;
}
#pragma endregion
