#include "Bullet.h"
#include "AudioManager.h"

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
		color = ftxui::Color::HotPink;
	}

	sprite = Sprite(1, 1, { color });

	// 총 소리 출력
	AudioManager::GetInstance().PlayAudio(L"sfx_gunfire.wav", 0.15f);
}

void Bullet::Update()
{
	x -= speedX;
	y -= speedY;
}
