#pragma once
#include "GameObject.h"
#include "GameConstants.h"

class Explosion : public GameObject
{
private:
	static std::vector<Sprite> commonSprites;
	std::vector<Sprite> customSprites;

	int lifeTimeTick;
	int tick;
	int animationIndex;
	bool isCommonSize;

	void SetSprites(int w, int h);

public:
	Explosion(int x, int y, int w = EXPLOSION_DEFAULT_SIZE_X, int h = EXPLOSION_DEFAULT_SIZE_Y);

	void Reset(int x, int y, int w = EXPLOSION_DEFAULT_SIZE_X, int h = EXPLOSION_DEFAULT_SIZE_Y);
	void Update() override;
	void OnDestroy(std::shared_ptr<GameObject> self) override;

	static void LoadSprites(std::vector<Sprite>& sprites, int w, int h);
};

