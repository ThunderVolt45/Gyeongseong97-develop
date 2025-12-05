#pragma once
#include "GameObject.h"
#include <functional>

class Bullet : public GameObject
{
public: // Protected -> Public for flexibility in callbacks
	int damage;
	float speedX;
	float speedY;

public:
	bool isPlayer;
	bool isExplosive;

	// Custom behavior callbacks
	std::function<void(Bullet*)> onUpdate;
	std::function<void(Bullet*)> onDestroy;

	Bullet();
	Bullet(int x, int y, float speedX, float speedY, bool isMine, int damage = 1);

	virtual void Reset(int x, int y, float speedX, float speedY, bool isMine, int damage = 1);
	
	// New method to set custom behavior
	void SetCustomBehavior(Sprite sprite, std::function<void(Bullet*)> onUpdate, std::function<void(Bullet*)> onDestroy);

	void Update() override;
	void Destroy() override;
	int GetDamage();
	void OnDestroy(std::shared_ptr<GameObject> self) override;
};

