#pragma once
#include "GameObject.h"
#include "ImageLoader.h"

class Player : public GameObject
{
private:
	Sprite defaultSprite;
	Sprite deathSprite;

	void Destroy();

public:
	float maxHealth;
	float health;
	int cooldown;

	Player();
	Player(int x, int y);

	void Reset();
	void Update() override;
	void OnCollision(GameObject& other) override;
};

