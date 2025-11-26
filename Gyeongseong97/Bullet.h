#pragma once
#include "GameObject.h"

class Bullet : public GameObject
{
private:
	float speedX;
	float speedY;
	bool isMine;

public:
	Bullet(int x, int y, float speedX, float speedY, bool isMine);
	void Update() override;
};

