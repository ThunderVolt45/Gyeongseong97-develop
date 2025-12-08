#include "Weapon.h"
#include "Player.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "ObjectPool.h"
#include "GameConstants.h"
#include "Enums.h"

void Weapon::AddAmmo(int amount)
{
	remainBullet += amount;
}

// ==========================
// WeaponDefault
// ==========================
WeaponDefault::WeaponDefault()
	: Weapon(WeaponType::Default, DEFAULT_WEAPON_DAMAGE, 9999, DEFAULT_WEAPON_COOLTIME, true)
{
	// 450RPM의 무난무난한 기본 무기
}

void WeaponDefault::Shoot(Player* owner)
{
	// 기존 플레이어의 기본 발사 로직
	GameManager& gameManager = GameManager::GetInstance();

	std::shared_ptr<Bullet> bullet = ObjectPool<Bullet>::GetInstance().Get(
		owner->GetCenterX(),
		owner->y,
		0.0f,
		6.0f,
		true,
		damage
	);

	gameManager.CreateGameObject(bullet, TargetLayer::Foreground);
	AudioManager::GetInstance().PlayAudio(SFX_GUNFIRE.data(), 0.15f);
}