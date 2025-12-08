#include "WeaponRocket.h"
#include "Player.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "ImageLoader.h"
#include "BulletPool.h"
#include "GameConstants.h"

#include <cmath>
#include <algorithm>

WeaponRocket::WeaponRocket()
	: Weapon(WeaponType::Rocket, 3, 30, 25, false)
{
	// 높은 피해량과 호밍 능력을 갖지만 탄이 적고 연사가 느린 로켓런처
}

void WeaponRocket::Shoot(Player* owner)
{
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

	// Start slightly slower than normal bullets
	float initialSpeed = 4.0f;

	std::shared_ptr<Bullet> bullet = bulletPool.GetBullet(
		owner->GetCenterX(),
		owner->y,
		0.0f,           // Initial X speed
		initialSpeed,   // Initial Y speed (Moves UP because Bullet::Update does y -= speedY)
		true,
		damage
	);

	bullet->SetCustomBehavior(sprite, &WeaponRocket::RocketHomingBehavior, nullptr);

	gameManager.CreateGameObject(bullet);
	AudioManager::GetInstance().PlayAudio(SFX_GUNFIRE.data(), 0.15f); // Maybe need a rocket sound later
}

void WeaponRocket::RocketHomingBehavior(Bullet* b)
{
	GameManager& gm = GameManager::GetInstance();
	
	GameObject* target = nullptr;
	float minDist = 999999.0f;
	
	// 가장 가까이에 있는 Enemy 오브젝트를 찾는다
	// Note: We need to be careful about thread safety if this runs on a separate thread,
	// but usually Update() happens in the logic thread where gameObjects is stable-ish 
	// or locked by GameManager's update. 
	// However, safely accessing the list is best.
	{
		std::lock_guard<std::recursive_mutex> lock(gm.gameMutex);
		for (auto& obj : gm.gameObjects)
		{
			// dynamic_cast를 이용해 Enemy인지 검사한다
			if (auto enemy = std::dynamic_pointer_cast<Enemy>(obj))
			{
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

	// 로켓의 속도를 설정한다
	float speed = 4.0f;

	if (target)
	{
		// Calculate vector to target
		float dx = target->GetCenterX() - b->GetCenterX();
		float dy = target->GetCenterY() - b->GetCenterY();
		float length = std::sqrt(dx * dx + dy * dy);

		if (length > 0)
		{
			// Normalize
			float dirX = dx / length;
			float dirY = dy / length;

			// Bullet::Update does: x -= speedX, y -= speedY
			// To move TOWARDS target (dx, dy), we need speed to be NEGATIVE of direction
			// e.g. Target is to the right (dx > 0). x should increase. 
			// x -= speedX  =>  x -= (-val)  => x += val.
			
			// We want smooth steering, but for now, let's use direct homing with a turn cap 
			// or just direct tracking for simplicity. 
			// Let's do direct tracking for the "Rocket" feel.
			
			// Introduce a "Turn Rate" to make it curve instead of snap
			float targetSpeedX = -dirX * speed;
			float targetSpeedY = -dirY * speed;

			// Simple lerp for steering (0.1f is turn speed)
			b->speedX = b->speedX + (targetSpeedX - b->speedX) * 0.15f;
			b->speedY = b->speedY + (targetSpeedY - b->speedY) * 0.15f;
		}
	}
	else
	{
		// No target? Just keep going straight (accelerate slightly?)
		// Or just maintain current velocity.
		// 타겟을 찾지 못했다면 그냥 직진
	}

	// 조금씩 가속한다


	// 로켓을 움직여준다
	// (익명함수가 기본 Update를 override하므로 직접 수행해야함)
	b->x -= b->speedX;
	b->y -= b->speedY;
}
