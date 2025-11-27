#pragma once
#include "GameObject.h"

class Explosion : public GameObject
{
private:
	static std::vector<Sprite> commonSprites;
	int lifeTimeTick;
	int tick;
	int animationIndex;

public:
	Explosion(int x, int y);
	void Update() override;

	static void LoadAssets();
};

