#include "Bullet.h"
#include "AudioManager.h"
#include "GameConstants.h"

Bullet::Bullet(int x, int y, float speedX, float speedY, bool isMine)
{
	this->x = x;
	this->y = y;
	this->speedX = speedX;
	this->speedY = speedY;
	this->isPlayer = isMine;

	ftxui::Color color = ftxui::Color::Yellow;

	if (!isMine)
	{
		color = ftxui::Color::Red1;
	}

	sprite = Sprite(1, 1, { color });

	// 총 소리 출력
	AudioManager::GetInstance().PlayAudio(SFX_GUNFIRE.data(), 0.1f);
}

void Bullet::Reset(int x, int y, float speedX, float speedY, bool isMine)
{
	this->x = x;
	this->y = y;
	this->speedX = speedX;
	this->speedY = speedY;
	this->isPlayer = isMine;

	ftxui::Color color = ftxui::Color::Yellow;

	if (!isMine)
	{
		color = ftxui::Color::Red1;
	}

	sprite = Sprite(1, 1, { color });

	// 총 소리 출력
	AudioManager::GetInstance().PlayAudio(SFX_GUNFIRE.data(), 0.1f);
}

void Bullet::Update()
{
	x -= speedX;
	y -= speedY;
}
