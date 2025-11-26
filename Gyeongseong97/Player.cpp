#define NOMINMAX // Prevent min/max macro conflicts with windows.h
#include <windows.h>

#include "Player.h"
#include "GameManager.h"
#include "Bullet.h"

extern const int GAME_WIDTH;
extern const int GAME_HEIGHT;

Player::Player() : GameObject()
{
	maxHealth = 5;
	health = 5;
	tick = 0;
	cooldown = 0;
}

Player::Player(int x, int y, int w, int h, std::wstring spriteName) : GameObject(x, y, w, h, spriteName)
{
	maxHealth = 5;
	health = 5;
	tick = 0;
	cooldown = 0;
}

void Player::Reset()
{

}

void Player::Update()
{
	tick++;

	// 사격 쿨다운
	if (cooldown > 0) cooldown--;

	// 키 입력 처리
	// 이동 (화살표 키)
	if ((GetAsyncKeyState(VK_LEFT) & 0x8000) && x > 2 - sprite.sizeX / 2) x -= 2;
	if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) && x < GAME_WIDTH - 2 - sprite.sizeX / 2) x += 2;
	if ((GetAsyncKeyState(VK_UP) & 0x8000) && y > 2 - sprite.sizeY / 2) y -= 1;
	if ((GetAsyncKeyState(VK_DOWN) & 0x8000) && y < GAME_HEIGHT - 2 - sprite.sizeY / 2) y += 1;

	// 발사 (Space 키)
	if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && cooldown <= 0)
	{
		Bullet bullet = Bullet(x + sprite.sizeX / 2, y - 3, 0, 4, true);
		GameManager::GetInstance().CreateBullet(bullet);

		cooldown = 6; // 6틱(약 100ms) 쿨다운
	}
}
