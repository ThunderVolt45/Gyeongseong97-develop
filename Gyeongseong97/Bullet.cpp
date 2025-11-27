#include "Bullet.h"

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
}

void Bullet::Update()
{
	x -= speedX;
	y -= speedY;
}
