#include "BossStateAttackBomb.h"
#include "Narration.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "BulletPool.h"
#include "ExplosionPool.h"

void BossStateAttackBomb::Update(Narration& boss)
{
	boss.internalTick++;

	// 일정 틱 동안 대기
	if (boss.internalTick < 60)
	{
		return;
	}

	// 고폭탄 발사 준비
	GameManager& gameManager = GameManager::GetInstance();
	BulletPool& bulletPool = BulletPool::GetInstance();
	ExplosionPool& explosionPool = ExplosionPool::GetInstance();

	// Sprite 생성
	auto c = ftxui::Color::DarkRed;
	auto sprite = Sprite(4, 4,
		{
			c, c, c, c,
			c, c, c, c,
			c, c, c, c,
			c, c, c, c,
		});

	// 폭★8
	auto explosion = [](Bullet* b) {
		GameManager& gameManager = GameManager::GetInstance();
		BulletPool& bulletPool = BulletPool::GetInstance();
		ExplosionPool& explosionPool = ExplosionPool::GetInstance();

		// 파편 효과 생성 (총알)
		// 8방향으로 총알 발사
		for (int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				if (x == 0 && y == 0) continue;

				std::shared_ptr<Bullet> bullet = bulletPool.GetBullet(
					b->GetCenterX(),
					b->GetCenterY(),
					6.0f * x,
					6.0f * y,
					false
				);

				gameManager.CreateGameObject(bullet);
			}
		}

		// 폭발 효과 생성 (데미지 전달)
		std::shared_ptr<Explosion> explosion =
			explosionPool.GetExplosion(b->GetCenterX(), b->GetCenterY(), 60, 45, 1, false);

		gameManager.CreateGameObject(explosion, false);
		};

	// 플레이어와 나레이션 사이의 방향 벡터를 구한다 (Bullet은 x -= speedX 이므로 boss - player로 계산)
	float dx = boss.GetCenterX() - gameManager.player.GetCenterX();
	float dy = boss.GetCenterY() - gameManager.player.GetCenterY();
	
	// 정규화
	float dist = std::sqrt(dx * dx + dy * dy);
	if (dist > 0)
	{
		dx /= dist;
		dy /= dist;
	}

	// 속도 적용
	float speed = 2.0f;
	dx *= speed;
	dy *= speed;

	std::shared_ptr<Bullet> bomb = BulletPool::GetInstance().GetCustomBullet(
		boss.GetCenterX(),
		boss.GetCenterY(),
		dx,
		dy,
		false,
		2,
		60,
		sprite,
		nullptr,
		explosion
	);

	// 폭탄 생성
	gameManager.CreateGameObject(bomb);
	AudioManager::GetInstance().PlayAudio(SFX_GRENADE.data(), 0.2f);

	// 다음 패턴으로 넘어간다
	boss.ChangePattern();
}
