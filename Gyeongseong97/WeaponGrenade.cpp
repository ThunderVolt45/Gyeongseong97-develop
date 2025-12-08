#include "WeaponGrenade.h"
#include "Player.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "BulletPool.h"
#include "ExplosionPool.h"
#include "GameConstants.h"
#include "Enums.h"

WeaponGrenade::WeaponGrenade()
	: Weapon(WeaponType::Grenade, GRENADE_DAMAGE, 10, GRENADE_COOLTIME, false)
{
	// 0.5초 간격으로 나가는 강력한 유★탄발사기
}

void WeaponGrenade::Shoot(Player* owner)
{
	if (remainBullet <= 0) return;

	remainBullet--;

	// 발사 로직
	GameManager& gameManager = GameManager::GetInstance();
	BulletPool& bulletPool = BulletPool::GetInstance();

	// Sprite 생성
	auto c = ftxui::Color::DarkGreen;
	auto sprite = Sprite(4, 4,
		{
			c, c, c, c,
			c, c, c, c,
			c, c, c, c,
			c, c, c, c,
		});

	// 커스텀 총알 생성
	std::shared_ptr<Bullet> bullet = bulletPool.GetCustomBullet(
		owner->GetCenterX(),
		owner->y,
		0.0f,
		4.5f,
		true,
		damage,
		-1,
		sprite,
		nullptr,
		&WeaponGrenade::ExplosionBehavior
	);

	gameManager.CreateGameObject(bullet, TargetLayer::Foreground);
	AudioManager::GetInstance().PlayAudio(SFX_GRENADE.data(), 0.2f);
}

void WeaponGrenade::ExplosionBehavior(Bullet* b)
{
	// 발사 로직
	GameManager& gameManager = GameManager::GetInstance();
	BulletPool& bulletPool = BulletPool::GetInstance();

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
				true
			);

			gameManager.CreateGameObject(bullet);
		}
	}

	// 폭발 효과 생성 (데미지 전달)
	std::shared_ptr<Explosion> explosion =
		ExplosionPool::GetInstance().GetExplosion(b->GetCenterX(), b->GetCenterY(), 60, 45, b->damage, true);

	gameManager.CreateGameObject(explosion, TargetLayer::Background);
}
