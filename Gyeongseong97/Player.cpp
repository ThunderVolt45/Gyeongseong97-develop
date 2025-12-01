#define NOMINMAX // Prevent min/max macro conflicts with windows.h
#include <windows.h>

#include "GameManager.h"
#include "Player.h"
#include "Bullet.h"
#include "GameConstants.h"

Player::Player() : GameObject()
{
	maxHealth = 5;
	health = 5;
	cooldown = 0;
}

Player::Player(int x, int y, int w, int h, std::wstring spriteName) : GameObject(x, y, w, h, spriteName)
{
	maxHealth = 5;
	health = 5;
	cooldown = 0;
}

void Player::Reset()
{
	cooldown = 0;
	health = maxHealth;
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
	// 만약 Bullet과 충돌했다면
	Bullet* bullet = dynamic_cast<Bullet*>(&other);
	if (bullet)
	{
		if (!bullet->isPlayer)
		{
			health -= 1;
		}

		// 총알 파괴
		GameManager::GetInstance().DestroyGameObject(bullet);

		return;
	}

	// 만약 Enemy와 충돌했다면
	Enemy* enemy = dynamic_cast<Enemy*>(&other);
	if (enemy)
	{
		health -= 0.1f;
	}
}
