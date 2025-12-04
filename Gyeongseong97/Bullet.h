#pragma once
#include "GameObject.h"

class Bullet : public GameObject
{
protected:
	int damage;
	float speedX;
	float speedY;

public:
	bool isPlayer;

	Bullet();
	Bullet(int x, int y, float speedX, float speedY, bool isMine, int damage = 1);

	virtual void Reset(int x, int y, float speedX, float speedY, bool isMine, int damage = 1);
	void Update() override;
	int GetDamage();
	void OnDestroy(std::shared_ptr<GameObject> self) override;
};

