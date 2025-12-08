#include "WeaponRocket.h"
#include "Player.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "ImageLoader.h"
#include "BulletPool.h"
#include "ExplosionPool.h"
#include "GameConstants.h"

#include <cmath>
#include <algorithm>

WeaponRocket::WeaponRocket()
	: Weapon(WeaponType::Rocket, ROCKET_DAMAGE, 30, ROCKET_COOLTIME, false)
{
	// 높은 피해량과 호밍 능력을 갖지만 탄이 적고 연사가 느린 로켓런처
}

void WeaponRocket::Shoot(Player* owner)
{
	if (remainBullet <= 0) return;

	remainBullet--;

	GameManager& gameManager = GameManager::GetInstance();
	BulletPool& bulletPool = BulletPool::GetInstance();
	
	// Sprite 생성
	auto w = ftxui::Color::White;
	auto y = ftxui::Color::Yellow;
	Sprite sprite = Sprite(4, 16, 
		{
			w, w, w, w,
			w, w, w, w,
			w, w, w, w,
			w, w, w, w,
			y, y, y, y,
			y, y, y, y,
			w, w, w, w,
			w, w, w, w,
			w, w, w, w,
			w, w, w, w,
			w, w, w, w,
			w, w, w, w,
			w, w, w, w,
			w, w, w, w,
			w, w, w, w,
			w, w, w, w
		}
	);

	std::shared_ptr<Bullet> bullet = bulletPool.GetBullet(
		owner->GetCenterX(),
		owner->y,
		0.0f,
		2.0f,
		true,
		damage,
		ROCKET_MAX_LIFETICK
	);

	bullet->SetCustomBehavior(sprite, &WeaponRocket::RocketHomingBehavior, &WeaponRocket::ExplosionBehavior);
	gameManager.CreateGameObject(bullet, TargetLayer::Foreground);

	// 로켓 소리
	AudioManager::GetInstance().PlayAudio(SFX_ROCKET.data(), 0.15f);
}

void WeaponRocket::RocketHomingBehavior(Bullet* b)
{
	GameManager& gm = GameManager::GetInstance();
	
	GameObject* target = nullptr;
	float minDist = 999999.0f;
	
	// 가장 가까이에 있는 Enemy 오브젝트를 찾는다
	{
		std::lock_guard<std::recursive_mutex> lock(gm.gameMutex);
		for (auto& obj : gm.gameObjects)
		{
			// dynamic_cast를 이용해 Enemy인지 검사한다
			if (auto enemy = std::dynamic_pointer_cast<Enemy>(obj))
			{
				// 로켓 보다 뒤에 있는 목표는 고려하지 않는다
				if (enemy->GetCenterY() > b->GetCenterY()) continue;
				
				// 거리 계산
				float dx = enemy->GetCenterX() - b->GetCenterX();
				float dy = enemy->GetCenterY() - b->GetCenterY();
				float dist = std::sqrt(dx * dx + dy * dy);

				// 마지막으로 찾은 Enemy보다 더 가까이에 있다면 목표를 변경
				if (dist < minDist)
				{
					minDist = dist;
					target = enemy.get();
				}
			}
		}
	}

	// 로켓의 Y축 가속
	// 수명이 줄어들수록(시간이 지날수록) 빨라진다.
	float speed = 2.0f + (float)(ROCKET_MAX_LIFETICK - b->lifeTick) / (ROCKET_MAX_LIFETICK / 4);
	b->speedY = speed;

	// X축 추적 (X축으로만 이동)
	if (target)
	{
		// 내 위치와 타겟의 X 위치 차이
		float dx = b->GetCenterX() - target->GetCenterX();
		
		// Bullet::Update는 x -= speedX를 수행함.
		// 타겟이 내 왼쪽(dx > 0) -> 왼쪽으로 이동해야 함 -> speedX > 0
		// 타겟이 내 오른쪽(dx < 0) -> 오른쪽으로 이동해야 함 -> speedX < 0
		
		float targetSpeedX = 0.0f;
		float maxSteerSpeed = speed / 2; // 최대 횡이동 속도

		if (dx > 5.0f) // 타겟이 확실히 왼쪽에 있음 (오차 범위 5)
		{
			targetSpeedX = maxSteerSpeed;
		}
		else if (dx < -5.0f) // 타겟이 확실히 오른쪽에 있음
		{
			targetSpeedX = -maxSteerSpeed;
		}
		
		// 부드러운 회전을 위해 현재 속도에서 목표 속도로 보간(Lerp)
		float steerStrength = 0.05f;
		b->speedX = b->speedX + (targetSpeedX - b->speedX) * steerStrength;
	}
	else
	{
		// 타겟이 없으면 X축 속도를 서서히 줄여 직진하게 만듦
		b->speedX *= 0.95f;
	}

	// 로켓을 움직여준다
	b->x -= b->speedX;
	b->y -= b->speedY;
}

void WeaponRocket::ExplosionBehavior(Bullet* b)
{
	GameManager& gameManager = GameManager::GetInstance();

	// 전방 폭발 (즉시 생성)
	std::shared_ptr<Explosion> explosionForward =
		ExplosionPool::GetInstance().GetExplosion(b->GetCenterX(), b->GetCenterY(), 40, 30, b->damage, true);
	gameManager.CreateGameObject(explosionForward, TargetLayer::Background);

	// 후방 폭발 (지연 생성)을 위한 Timer(Fuse) Bullet 생성
	BulletPool& bulletPool = BulletPool::GetInstance();
	
	std::shared_ptr<Bullet> fuse = bulletPool.GetBullet(
		b->GetCenterX(), // 위치 유지
		b->GetCenterY(),
		0.0f, 
		0.0f, // 속도 0 (움직이지 않음)
		true,
		b->damage,
		5 // 지연 시간
	);

	// fuse는 보이지 않게 한다
	Sprite emptySprite = Sprite(0, 0, {});

	fuse->SetCustomBehavior(emptySprite, nullptr, &WeaponRocket::DelayedExplosion);
	gameManager.CreateGameObject(fuse);
}

void WeaponRocket::DelayedExplosion(Bullet* fuse)
{
	// 지연된 후방 폭발 생성
	// fuse의 위치를 기준으로 생성 (fuse는 움직이지 않았으므로 원본 위치와 동일)
	std::shared_ptr<Explosion> explosionBackward =
		ExplosionPool::GetInstance().GetExplosion(fuse->GetCenterX(), fuse->GetCenterY() - 30, 60, 45, fuse->damage, true);

	GameManager::GetInstance().CreateGameObject(explosionBackward, TargetLayer::Background);
}
