#include "GameManager.h"
#include "GameConstants.h"
#include "Player.h"
#include "BulletPool.h"
#include "ExplosionPool.h"
#include "InputManager.h"

Player::Player() : GameObject()
{
	maxHealth = 5;
	health = 5;
	cooldown = 0;
	invincible = false;
	
	// 기본 무기 초기화
	defaultWeapon = std::make_shared<WeaponDefault>();
	currentWeapon = defaultWeapon;

	// 수류탄 초기화
	grenadeWeapon = std::make_shared<WeaponGrenade>();
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

	// 수류탄 초기화
	grenadeWeapon = std::make_shared<WeaponGrenade>();
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

	// 수류탄 초기화
	grenadeWeapon = std::make_shared<WeaponGrenade>();
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
	InputManager& inputManager = InputManager::GetInstance();

	// 이동 (화살표 키)
	if (inputManager.IsKeyDown(KEYCODE_LEFT) && x > 2 - sprite.sizeX / 2) x -= 3;
	if (inputManager.IsKeyDown(KEYCODE_RIGHT) && x < GAME_WIDTH - 2 - sprite.sizeX / 2) x += 3;
	if (inputManager.IsKeyDown(KEYCODE_UP) && y > 2 - sprite.sizeY / 2) y -= 2;
	if (inputManager.IsKeyDown(KEYCODE_DOWN) && y < GAME_HEIGHT - 2 - sprite.sizeY / 2) y += 2;

	// 발사 (Space 키) -> Shoot() 함수 호출로 변경
	if (inputManager.IsKeyDown(KEYCODE_SPACE) && cooldown <= 0)
	{
		Shoot();
	}

	// 수류탄 (Z 키) 연사 X
	if (inputManager.IsKeyPressed(KEYCODE_Z))
	{
		Grenade();
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

void Player::Grenade()
{
	if (grenadeWeapon)
	{
		grenadeWeapon->Shoot(this);
	}
}

void Player::EquipWeapon(std::shared_ptr<Weapon> newWeapon)
{
	auto grenade = dynamic_cast<WeaponGrenade*>(newWeapon.get());
	if (grenade)
	{
		grenadeWeapon.get()->AddAmmo(newWeapon.get()->remainBullet);
		return;
	}

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
		// 자신의 총알이면 무시
		if (bullet->isPlayer) return;

		// 데미지를 받는다
		TakeDamage(bullet->GetDamage());

		// 총알 파괴
		bullet->Destroy();

		return;
	}

	// 만약 Enemy와 충돌했다면
	Enemy* enemy = dynamic_cast<Enemy*>(&other);
	if (enemy)
	{
		TakeDamage(COLLISION_DAMAGE);
	}
}

void Player::TakeDamage(float damage)
{
	health -= damage;

	if (health <= 0 && !GameManager::GetInstance().IsGameOver)
	{
		Destroy();
	}
}