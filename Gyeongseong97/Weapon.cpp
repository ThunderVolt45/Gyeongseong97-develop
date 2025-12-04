#include "Weapon.h"
#include "Player.h"
#include "BulletPool.h"
#include "GameManager.h"
#include "GameConstants.h"
#include "AudioManager.h"
#include "Utility.h"

// ==========================
// WeaponDefault 기본 무기
// ==========================
WeaponDefault::WeaponDefault() 
	: Weapon(WeaponType::Default, 9999, 12, true) // 무한 탄창
{
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
		true
	);

	gameManager.CreateGameObject(bullet);
}

// ==========================
// WeaponHMG 헤비머신건
// ==========================
WeaponHMG::WeaponHMG()
	: Weapon(WeaponType::HMG, 200, 5, false)
{
	// 200발을 720RPM으로 화끈하게 때려붓는 헤비머신건!
}

void WeaponHMG::Shoot(Player* owner)
{
	if (remainBullet > 0)
	{
		remainBullet--;

		// 발사 로직
		GameManager& gameManager = GameManager::GetInstance();
		BulletPool& bulletPool = BulletPool::GetInstance();

		// Bullet의 X축 속도를 랜덤하게 부여하고 발사
		float speedX = (float)Utility::GenerateRandomNumber(-5, 5) / 10;

		std::shared_ptr<Bullet> bullet = bulletPool.GetBullet(
			owner->GetCenterX(),
			owner->y,
			speedX,
			6.0f,
			true
		);

		gameManager.CreateGameObject(bullet);
	}
}

// ==========================
// WeaponGrenade 이것은 수류탄이여!
// ==========================
WeaponGrenade::WeaponGrenade()
	: Weapon(WeaponType::Grenade, 20, 30, false) // 20발
{
	// 0.5초 간격으로 나가는 강력한 폭★8물
}

void WeaponGrenade::Shoot(Player* owner)
{
	// 추후 구현
	if (remainBullet > 0)
	{
		remainBullet--;

		// TODO: Grenade 발사 로직 구현
		std::cout << "Grenade Shot! Remain: " << remainBullet << std::endl;
	}
}
