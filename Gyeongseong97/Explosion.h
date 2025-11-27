#pragma once
#include "GameObject.h"

class Explosion : public GameObject
{
private:
	std::vector<Sprite> sprites;
	int lifeTimeTick;
	int tick;
	int animationIndex;

public:
	Explosion(int x, int y);
	void Update() override;
};

