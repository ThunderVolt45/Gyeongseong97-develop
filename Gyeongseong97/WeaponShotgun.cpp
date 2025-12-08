#include "WeaponShotgun.h"
#include "Player.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "ObjectPool.h"
#include "GameConstants.h"

WeaponShotgun::WeaponShotgun() :
	Weapon(WeaponType::Shotgun, SHOTGUN_DAMAGE_FOR_PELLET, 30, SHOTGUN_COOLTIME, false)
{
	// 0.5초 간격으로 여러 발의 Bullet을 한번에 쏘는 산탄총
}

void WeaponShotgun::Shoot(Player* owner)
{
	if (remainBullet <= 0) return;

	remainBullet--;

	// 발사 로직
	GameManager& gameManager = GameManager::GetInstance();

	// 한번에 5발 정도? 발사
	for (int i = -2; i <= 2; i++)
	{
		std::shared_ptr<Bullet> bullet = ObjectPool<Bullet>::GetInstance().Get(
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