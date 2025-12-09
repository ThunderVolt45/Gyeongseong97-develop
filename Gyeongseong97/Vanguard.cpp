#include "GameManager.h"
#include "AudioManager.h"
#include "GameConstants.h"
#include "Vanguard.h"
#include "Bullet.h"
#include "ObjectPool.h"
#include "Enums.h"
#include "Army.h"

#pragma region Constructer & Destroyer

Vanguard::Vanguard()
{

}

Vanguard::Vanguard(int x, int y, int health, float speed, int killScore)
{
	type = ObjectType::Enemy;

	cooldown = 30; // 등장하자마자 사격하지 못하게 한다

	this->health = health;
	this->speed = speed;
	this->killScore = killScore;

	originalSprite = ImageLoader::CreateSpriteFromImage(SPR_VANGUARD.data(), 18, 36);
	sprite = originalSprite;

	hitSprite = ImageLoader::CreateHitSprite(sprite);

	this->x = (float)(x - (sprite.sizeX / 2));
	this->y = (float)(y - (sprite.sizeY / 2));
}

#pragma endregion

#pragma region Public

void Vanguard::Update()
{
	ProcessHitEffect();

	y += speed / 4;
	if (cooldown > 0) cooldown -= 1;
	if (stunTime > 0) stunTime -= 1;

	Tracking();
	Attack();
}

#pragma endregion

#pragma region AI (?)

void Vanguard::Tracking()
{
	// 사격 직후라면 중단
	if (stunTime > 0) return;

	// 플레이어와 전위대가 같은 x축에 서도록 이동한다
	float playerX = GameManager::GetInstance().player.GetCenterX();
	
	if (playerX > x)
	{
		// 플레이어가 전위대보다 오른쪽에 있다면
		x += speed;
	}
	else if (playerX < x)
	{
		// 플레이어가 전위대보다 왼쪽에 있다면
		x -= speed;
	}

	// 플레이어와 전위대가 x축으로 거의 일치하는 상태라면
	if (abs(playerX - x) < speed)
	{
		x = playerX;
	}
}

void Vanguard::Attack()
{
	// 아직 쿨다운 중이라면 중단
	if (cooldown > 0) return;

	// 총알 생성
	std::shared_ptr<Bullet> bullet = ObjectPool<Bullet>::GetInstance().Get(
		GetCenterX(), 
		GetCenterY(), 
		0.0f, 
		-1.5f, 
		false
	);

	// 발사!
	GameManager::GetInstance().CreateGameObject(bullet, TargetLayer::Foreground);
	AudioManager::GetInstance().PlayAudio(SFX_GUNFIRE.data(), 0.2f);

	cooldown += 90; // 90틱 마다 한번 공격
	stunTime += 30; // 사격 후 30틱 간 대기
}

#pragma endregion
