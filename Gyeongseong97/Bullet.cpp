#include "Bullet.h"

Bullet::Bullet(int x, int y, float speedX, float speedY, bool isMine)
{
	this->x = x;
	this->y = y;
	this->speedX = speedX;
	this->speedY = speedY;
	this->isMine = isMine;
}

void Bullet::Update()
{
	x -= speedX;
	y -= speedY;
}
