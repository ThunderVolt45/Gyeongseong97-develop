#include "Carmikaze.h"
#include "GameConstants.h"

Carmikaze::Carmikaze()
{

}

Carmikaze::Carmikaze(int x, int y, int health, float speed, int killScore)
{
	this->health = health;
	this->speed = speed;
	this->killScore = killScore;
	
	sprite = ImageLoader::CreateSpriteFromImage(L"enemy_Carmikaze.png", 80, 45);

	this->x = (float)(x - (sprite.sizeX / 2));
	this->y = (float)(y - (sprite.sizeY / 2));

	// 만약 오른쪽에서 생성된다면 sprite를 뒤집어준다
	if (x > GAME_WIDTH / 2)
	{
		this->direction = -1;
		sprite = ImageLoader::FlipSpriteX(sprite);
	}
	else
	{
		this->direction = 1;
	}
}

void Carmikaze::Update()
{
	x += speed * direction;
}
