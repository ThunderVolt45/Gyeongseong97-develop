#define NOMINMAX // Prevent min/max macro conflicts with windows.h
#include <windows.h>

#include "GameManager.h"
#include "GameConstants.h"
#include "Player.h"
#include "Bullet.h"
#include "Explosion.h"

Player::Player() : GameObject()
{
	maxHealth = 5;
	health = 5;
	cooldown = 0;
	invincible = false;
}

Player::Player(int x, int y)
{
	int w = 40;
	int h = 30;

	this->x = (float)(x - (w / 2));
	this->y = (float)(y - (h / 2));

	defaultSprite = ImageLoader::CreateSpriteFromImage(L"image.png", w, h);
	deathSprite = ImageLoader::CreateSpriteFromImage(L"player_death.png", 40, 30);
	sprite = defaultSprite;

	maxHealth = 5;
	health = 5;
	cooldown = 0;
	invincible = false;
}

void Player::Destroy()
{
	GameManager& gameManager = GameManager::GetInstance();

	std::shared_ptr<Explosion> explosion(new Explosion(GetCenterX(), GetCenterY()));
	gameManager.CreateGameObject(explosion, false);

	sprite = deathSprite;
}

void Player::Reset()
{
	cooldown = 0;
	health = maxHealth;
	invincible = false;

	sprite = defaultSprite;
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

	// 발사 (Space 키)
	if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && cooldown <= 0)
	{
		std::shared_ptr<Bullet> bullet(new Bullet(GetCenterX(), y, 0.0f, 6.0f, true));
		GameManager::GetInstance().CreateGameObject(bullet);

		cooldown = 6; // 6틱(약 100ms) 쿨다운
	}

	// 체력 회복 (초당 0.2)
	health += 0.2f / 60;
	if (health > maxHealth) health = maxHealth;
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
