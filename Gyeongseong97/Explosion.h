#pragma once
#include "GameObject.h"

class Explosion : public GameObject
{
private:
	static std::vector<Sprite> commonSprites;
	std::vector<Sprite> customSprites;

	int lifeTimeTick;
	int tick;
	int animationIndex;
	bool isCommonSize;

public:
	Explosion(int x, int y);
	Explosion(int x, int y, int w, int h);

	void Update() override;

	static void LoadSprites();
	void LoadCustomSizeSprites(int w, int h);
};

