#pragma once
#include "GameObject.h"

class Enemy : public GameObject
{
protected:
	Sprite originalSprite;
	Sprite hitSprite;

	int hitEffectTick = 0;

	void ProcessHitEffect();

public:
	float health = 1;
	float speed = 1;
	int killScore = 100;

	Enemy();
	Enemy(int x, int y, int health, float speed, int killScore);

	virtual void TakeDamage(int damage);
	void Update() override;
	void OnCollision(GameObject& other) override;
	void Destroy() override;
};

