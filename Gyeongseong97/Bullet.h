#pragma once
#include "GameObject.h"

#include <functional>

class Bullet : public GameObject
{
public:
	int damage;
	int lifeTick; // 총알의 수명
	float speedX;
	float speedY;
	bool isPlayer;

	// Bullet에 특수한 동작을 부여하기 위한 콜백 함수
	std::function<void(Bullet*)> onUpdate;
	std::function<void(Bullet*)> onDestroy;

	Bullet();
	Bullet(int x, int y, float speedX, float speedY, bool isMine, 
		int damage = 1, int lifeTick = -1);

	virtual void Reset(int x, int y, float speedX, float speedY, bool isMine, 
		int damage = 1, int lifeTick = -1);
	
	// Bullet에 고유한 동작을 부여하는 함수
	void SetCustomBehavior(
		std::function<void(Bullet*)> onUpdate, 
		std::function<void(Bullet*)> onDestroy);
	void SetCustomBehavior(Sprite sprite, 
		std::function<void(Bullet*)> onUpdate, 
		std::function<void(Bullet*)> onDestroy);

	void Update() override;
	void Destroy() override;
	int GetDamage();
	void OnDestroy(std::shared_ptr<GameObject> self) override;
};

