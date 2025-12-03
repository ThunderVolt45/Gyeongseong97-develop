#pragma once
#include "GameObject.h"

class Bullet : public GameObject
{
private:
	float speedX;
	float speedY;

public:
	bool isPlayer;

	Bullet(int x, int y, float speedX, float speedY, bool isMine);

	void Reset(int x, int y, float speedX, float speedY, bool isMine);
	void Update() override;
	void OnDestroy(std::shared_ptr<GameObject> self) override;
};

