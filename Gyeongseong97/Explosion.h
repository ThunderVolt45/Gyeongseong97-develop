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
	int damage; // 폭발 데미지
	bool isPlayer;

	void SetSprites(int w, int h);

public:
	Explosion(int x, int y, int w = EXPLOSION_DEFAULT_SIZE_X, int h = EXPLOSION_DEFAULT_SIZE_Y, int damage = 0, bool isPlayer = true);

	void Reset(int x, int y, int w = EXPLOSION_DEFAULT_SIZE_X, int h = EXPLOSION_DEFAULT_SIZE_Y, int damage = 0, bool isPlayer = true);
	void Update() override;
	void OnCollision(GameObject& other) override; // 충돌 처리 함수 오버라이드
	void OnDestroy(std::shared_ptr<GameObject> self) override;

	static void LoadSprites(std::vector<Sprite>& sprites, int w, int h);
};

