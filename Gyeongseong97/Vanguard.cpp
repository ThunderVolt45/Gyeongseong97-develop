#include "GameManager.h"
#include "GameConstants.h"
#include "Vanguard.h"
#include "Bullet.h"

#pragma region Constructer & Destroyer

Vanguard::Vanguard()
{

}

Vanguard::Vanguard(int x, int y, int health, float speed, int killScore)
{
	cooldown = 30; // 등장하자마자 사격하지 못하게 한다

	this->health = health;
	this->speed = speed;
	this->killScore = killScore;

	sprite = ImageLoader::CreateSpriteFromImage(L"enemy_vanguard.png", 12, 36);

	this->x = (float)(x - (sprite.sizeX / 2));
	this->y = (float)(y - (sprite.sizeY / 2));
}

#pragma endregion

#pragma region Public

void Vanguard::Update()
{
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

	// 발사!
	std::shared_ptr<Bullet> bullet(new Bullet(GetCenterX(), GetCenterY() + 2, 0.0f, -2.0f, false));
	GameManager::GetInstance().CreateGameObject(bullet);

	cooldown += 60; // 60틱 마다 한번 공격
	stunTime += 15; // 사격 후 15틱 간 대기
}

#pragma endregion
