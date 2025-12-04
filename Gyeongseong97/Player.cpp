#define NOMINMAX 
#include <windows.h>

#include "GameManager.h"
#include "GameConstants.h"
#include "Player.h"
#include "BulletPool.h"
#include "ExplosionPool.h"

Player::Player() : GameObject()
{
	maxHealth = 5;
	health = 5;
	cooldown = 0;
	invincible = false;
	
	// 기본 무기 초기화
	defaultWeapon = std::make_shared<WeaponDefault>();
	currentWeapon = defaultWeapon;
}

Player::Player(int x, int y)
{
	int w = 20;
	int h = 40;

	this->x = (float)(x - (w / 2));
	this->y = (float)(y - (h / 2));

	defaultSprite = ImageLoader::CreateSpriteFromImage(SPR_PLAYER.data(), w, h);
	deathSprite = ImageLoader::CreateSpriteFromImage(SPR_PLAYER_DEATH.data(), 40, 30);
	sprite = defaultSprite;

	maxHealth = 5;
	health = 5;
	cooldown = 0;
	invincible = false;

	// 기본 무기 초기화
	defaultWeapon = std::make_shared<WeaponDefault>();
	currentWeapon = defaultWeapon;
}

Player::~Player()
{
	
}

void Player::Destroy()
{
	GameManager& gameManager = GameManager::GetInstance();

	std::shared_ptr<Explosion> explosion = ExplosionPool::GetInstance().GetExplosion(GetCenterX(), GetCenterY());
	gameManager.CreateGameObject(explosion, false);

	sprite = deathSprite;
	x -= 10;
	y += 5;
}

void Player::Reset()
{
	cooldown = 0;
	health = maxHealth;
	invincible = false;

	sprite = defaultSprite;

	// 무기 초기화 (기본 무기로 복귀)
	currentWeapon = defaultWeapon;
}

void Player::Update()
{
	if (health < 0)
	{
		return;
	}

	// 사격 쿨다운
	if (cooldown > 0) cooldown--;

	// 키 입력 처리
	// 이동 (화살표 키)
	if ((GetAsyncKeyState(VK_LEFT) & 0x8000) && x > 2 - sprite.sizeX / 2) x -= 3;
	if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) && x < GAME_WIDTH - 2 - sprite.sizeX / 2) x += 3;
	if ((GetAsyncKeyState(VK_UP) & 0x8000) && y > 2 - sprite.sizeY / 2) y -= 2;
	if ((GetAsyncKeyState(VK_DOWN) & 0x8000) && y < GAME_HEIGHT - 2 - sprite.sizeY / 2) y += 2;

	// 발사 (Space 키) -> Shoot() 함수 호출로 변경
	if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && cooldown <= 0)
	{
		Shoot();
	}

	// 체력 회복 (초당 0.2)
	health += 0.2f / 60;
	if (health > maxHealth) health = maxHealth;
}

void Player::Shoot()
{
	// 사격 쿨다운 검사
	if (cooldown > 0)
	{
		return;
	}

	if (currentWeapon)
	{
		currentWeapon->Shoot(this);
		cooldown = currentWeapon->cooldownTick;

		// 총알이 다 떨어졌는지 확인 (기본 무기는 무한이라 IsEmpty()가 항상 false)
		if (currentWeapon->IsEmpty())
		{
			// 기본 무기로 교체
			currentWeapon = defaultWeapon;
		}
	}
}

void Player::EquipWeapon(std::shared_ptr<Weapon> newWeapon)
{
	// 새 무기 장착
	currentWeapon = newWeapon;
}

void Player::OnCollision(GameObject& other)
{
	if (invincible) return;

	GameManager& gameManager = GameManager::GetInstance();

	// 만약 Bullet과 충돌했다면
	Bullet* bullet = dynamic_cast<Bullet*>(&other);
	if (bullet)
	{
		if (!bullet->isPlayer)
		{
			health -= 1;
		}

		// 체력이 다 닳았으면 파괴
		if (health <= 0 && !gameManager.IsGameOver)
		{
			Destroy();
		}

		// 총알 파괴
		gameManager.DestroyGameObject(bullet);

		return;
	}

	// 만약 Enemy와 충돌했다면
	Enemy* enemy = dynamic_cast<Enemy*>(&other);
	if (enemy)
	{
		health -= 0.1f;

		// 체력이 다 닳았으면 파괴
		if (health <= 0 && !gameManager.IsGameOver)
		{
			Destroy();
		}
	}
}
