#include "Weapon.h"
#include "Player.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "Utility.h"
#include "BulletPool.h"
#include "ExplosionPool.h"
#include "GameConstants.h"

// ==========================
// WeaponDefault 기본 무기
// ==========================
WeaponDefault::WeaponDefault() 
	: Weapon(WeaponType::Default, 1, 9999, 8, true)
{
	// 450RPM의 무난무난한 기본 무기
}

void WeaponDefault::Shoot(Player* owner)
{
	// 기존 플레이어의 기본 발사 로직
	GameManager& gameManager = GameManager::GetInstance();
	BulletPool& bulletPool = BulletPool::GetInstance();

	std::shared_ptr<Bullet> bullet = bulletPool.GetBullet(
		owner->GetCenterX(),
		owner->y, 
		0.0f, 
		6.0f, 
		true,
		damage
	);

	gameManager.CreateGameObject(bullet);
	AudioManager::GetInstance().PlayAudio(SFX_GUNFIRE.data(), 0.15f);
}

void Weapon::AddAmmo(int amount)
{
	remainBullet += amount;
}

// ==========================
// WeaponHMG
// ==========================
WeaponHMG::WeaponHMG()
	: Weapon(WeaponType::HMG, 1, 300, 4, false)
{
	// 총알 300발을 900RPM으로 화끈하게 때려붓는 중기관총!
}

void WeaponHMG::Shoot(Player* owner)
{
	if (remainBullet <= 0) return;

	remainBullet--;

	// 발사 로직
	GameManager& gameManager = GameManager::GetInstance();
	BulletPool& bulletPool = BulletPool::GetInstance();

	// Bullet의 X축 속도를 랜덤하게 부여하고 발사
	float speedX = (float)Utility::GenerateRandomNumber(-6, 6) / 10;

	std::shared_ptr<Bullet> bullet = bulletPool.GetBullet(
		owner->GetCenterX(),
		owner->y,
		speedX,
		6.0f,
		true,
		damage
	);

	gameManager.CreateGameObject(bullet);
	AudioManager::GetInstance().PlayAudio(SFX_GUNFIRE.data(), 0.15f);
}

// ==========================
// WeaponGrenade 이것은 수류탄이여!
// ==========================
WeaponGrenade::WeaponGrenade()
	: Weapon(WeaponType::Grenade, 5, 10, 30, false)
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
	
	// 폭★8
	auto explosion = [&](Bullet* b) {
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
			ExplosionPool::GetInstance().GetExplosion(b->GetCenterX(), b->GetCenterY(), 60, 45, damage);

		gameManager.CreateGameObject(explosion, false);
		};

	// 커스텀 총알 생성
	std::shared_ptr<Bullet> bullet = bulletPool.GetCustomBullet(
		owner->GetCenterX(),
		owner->y,
		0.0f,
		4.5f,
		true,
		damage,
		sprite,
		nullptr,
		explosion
	);

	gameManager.CreateGameObject(bullet);
	AudioManager::GetInstance().PlayAudio(SFX_GRENADE.data(), 0.2f);
}

// ==========================
// WeaponShotgun 산탄총
// ==========================
WeaponShotgun::WeaponShotgun() :
	Weapon(WeaponType::Shotgun, 2, 30, 30, false)
{
	// 0.5초 간격으로 여러 발의 Bullet을 한번에 쏘는 산탄총
}

void WeaponShotgun::Shoot(Player* owner)
{
	if (remainBullet <= 0) return;

	remainBullet--;

	// 발사 로직
	GameManager& gameManager = GameManager::GetInstance();
	BulletPool& bulletPool = BulletPool::GetInstance();

	// 한번에 5발 정도? 발사
	for (int i = -2; i <= 2; i++)
	{
		std::shared_ptr<Bullet> bullet = bulletPool.GetBullet(
			owner->GetCenterX(),
			owner->y,
			0.0f + (0.4f * i),
			6.0f,
			true,
			damage
		);

		gameManager.CreateGameObject(bullet);
	}

	AudioManager::GetInstance().PlayAudio(SFX_GUNFIRE.data(), 0.4f);
}
