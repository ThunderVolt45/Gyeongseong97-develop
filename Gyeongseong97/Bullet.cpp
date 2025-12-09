#include "Bullet.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "Explosion.h"
#include "ObjectPool.h"
#include "GameConstants.h"

Bullet::Bullet()
{

}

Bullet::Bullet(int x, int y, float speedX, float speedY, bool isMine, int damage, int lifeTick)
{
	Reset(x, y, speedX, speedY, isMine, damage, lifeTick);
}

void Bullet::Reset(int x, int y, float speedX, float speedY, bool isMine, int damage, int lifeTick)
{
	type = ObjectType::Bullet;

	this->x = x;
	this->y = y;
	this->speedX = speedX;
	this->speedY = speedY;
	this->isPlayer = isMine;
	this->damage = damage;
	this->lifeTick = lifeTick;

	// 콜백 함수는 비워둔다
	onUpdate = nullptr;
	onDestroy = nullptr;

	AudioManager& audioManager = AudioManager::GetInstance();

	ftxui::Color color = ftxui::Color::Yellow;

	if (!isMine)
	{
		color = ftxui::Color::Red1;
	}

	sprite = Sprite(1, 1, { color });
}

void Bullet::SetCustomBehavior(std::function<void(Bullet*)> onUpdate = nullptr, std::function<void(Bullet*)> onDestroy = nullptr)
{
	this->onUpdate = onUpdate;
	this->onDestroy = onDestroy;
}

void Bullet::SetCustomBehavior(Sprite sprite, std::function<void(Bullet*)> onUpdate = nullptr, std::function<void(Bullet*)> onDestroy = nullptr)
{
	this->sprite = sprite;
	this->onUpdate = onUpdate;
	this->onDestroy = onDestroy;
}

void Bullet::Update()
{
	// 수명 처리
	if (lifeTick > 0)
	{
		lifeTick--;

		if (lifeTick == 0)
		{
			Destroy();
		}
	}

	// 로직 실행
	if (onUpdate)
	{
		onUpdate(this);
	}
	else
	{
		x -= speedX;
		y -= speedY;
	}
}

int Bullet::GetDamage()
{
	return damage;
}

void Bullet::Destroy()
{
	if (onDestroy)
	{
		onDestroy(this);
	}

	GameObject::Destroy();
}

void Bullet::OnDestroy(std::shared_ptr<GameObject> self)
{
	// 오브젝트 풀에 반환
	if (auto bullet = std::dynamic_pointer_cast<Bullet>(self))
	{
		ObjectPool<Bullet>::GetInstance().Return(bullet);
	}
}
