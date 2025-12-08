#include "WeaponItem.h"
#include "Player.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "GameConstants.h"
#include "ImageLoader.h"
#include "Weapon.h"
#include "WeaponHMG.h"
#include "WeaponShotgun.h"
#include "WeaponGrenade.h"
#include "WeaponRocket.h"

WeaponItem::WeaponItem(float x, float y, float speed, WeaponType type) :
	weaponType(type), speed(speed)
{
	// 타입에 따라 다른 이미지 로드
	std::wstring spriteName;

	switch (type)
	{
	case WeaponType::Shotgun:
		spriteName = SPR_SHOTGUN.data();
		break;
	case WeaponType::Grenade:
		spriteName = SPR_BOMB.data();
		break;
	case WeaponType::HMG:
	case WeaponType::Default:
	default:
		spriteName = SPR_HMG.data();
		break;
	}

	sprite = ImageLoader::CreateSpriteFromImage(spriteName, 22, 20); 

	this->x = x - sprite.sizeX;
	this->y = y - sprite.sizeY;
}

void WeaponItem::Update()
{
	y += speed;

	if (y > GAME_HEIGHT)
	{
		GameManager::GetInstance().DestroyGameObject(this);
	}
}

void WeaponItem::OnCollision(GameObject& other)
{
	Player* player = dynamic_cast<Player*>(&other);
	if (player)
	{
		// 무기 생성
		std::shared_ptr<Weapon> newWeapon;

		switch (weaponType)
		{
		case WeaponType::HMG:
			newWeapon = std::make_shared<WeaponHMG>();
			break;
		case WeaponType::Shotgun:
			newWeapon = std::make_shared<WeaponShotgun>();
			break;
		case WeaponType::Grenade:
			newWeapon = std::make_shared<WeaponGrenade>();
			break;
		case WeaponType::Rocket:
			newWeapon = std::make_shared<WeaponRocket>();
			break;
		default:
			newWeapon = std::make_shared<WeaponDefault>();
			break;
		}

		// 플레이어에게 장착
		if (newWeapon)
		{
			player->EquipWeapon(newWeapon);
		}

		// 아이템 제거
		GameManager::GetInstance().DestroyGameObject(this);
		AudioManager::GetInstance().PlayAudio(SFX_GETWEAPON.data());
	}
}