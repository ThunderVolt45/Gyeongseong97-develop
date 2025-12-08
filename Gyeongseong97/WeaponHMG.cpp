#include "WeaponHMG.h"
#include "Player.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "ObjectPool.h"
#include "Utility.h"
#include "Bullet.h"
#include "GameConstants.h"
#include "Enums.h"

WeaponHMG::WeaponHMG()
	: Weapon(WeaponType::HMG, HMG_DAMAGE, 300, HMG_COOLTIME, false)
{
	// 총알 300발을 900RPM으로 화끈하게 때려붓는 중기관총!
}

void WeaponHMG::Shoot(Player* owner)
{
	if (remainBullet <= 0) return;

	remainBullet--;

	// 발사 로직
	GameManager& gameManager = GameManager::GetInstance();

	// Bullet의 X축 속도를 랜덤하게 부여하고 발사
	float speedX = (float)Utility::GenerateRandomNumber(-6, 6) / 10;

	std::shared_ptr<Bullet> bullet = ObjectPool<Bullet>::GetInstance().Get(
		owner->GetCenterX(),
		owner->y,
		speedX,
		6.0f,
		true,
		damage
	);

	gameManager.CreateGameObject(bullet, TargetLayer::Foreground);
	AudioManager::GetInstance().PlayAudio(SFX_GUNFIRE.data(), 0.15f);
}