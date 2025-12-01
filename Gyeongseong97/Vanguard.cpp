#include "Vanguard.h"
#include "GameConstants.h"

Vanguard::Vanguard()
{

}

Vanguard::Vanguard(int x, int y, int health, float speed, int killScore)
{
	this->x = (float)(x - (sprite.sizeX / 2));
	this->y = (float)(y - (sprite.sizeY / 2));
	this->health = health;
	this->speed = speed;
	this->killScore = killScore;

	this->direction = x < GAME_WIDTH / 2 ? 1 : -1;
}

void Vanguard::Update()
{

}