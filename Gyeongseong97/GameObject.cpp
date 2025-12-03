#include "GameObject.h"
#include "ImageLoader.h"
#include "GameConstants.h"

GameObject::GameObject()
{

}

GameObject::GameObject(int x, int y)
{
	this->x = x;
	this->y = y;
}

GameObject::GameObject(int x, int y, int w, int h, std::wstring spriteName)
{
	this->x = (float)(x - (w / 2));
	this->y = (float)(y - (h / 2));

	sprite = ImageLoader::CreateSpriteFromImage(spriteName, w, h);
}

GameObject::~GameObject()
{

}

float GameObject::GetCenterX()
{
	return x + sprite.sizeX / 2;
}

float GameObject::GetCenterY()
{
	return y + sprite.sizeY / 2;
}

void GameObject::Update()
{

}

void GameObject::OnDestroy(std::shared_ptr<GameObject> self)
{
	// 기본적으로는 아무것도 하지 않는다.
}

bool GameObject::IsOutOfScreen()
{
	bool isOutofScreen = false;

	int w = 0;
	int h = 0;

	if (sprite.sizeX)
	{
		w = sprite.sizeX;
		h = sprite.sizeY;
	}

	if (x + w < 0) isOutofScreen = true;
	if (x - w > GAME_WIDTH) isOutofScreen = true;
	if (y + h < 0) isOutofScreen = true;
	if (y - h > GAME_HEIGHT) isOutofScreen = true;

	return isOutofScreen;
}

bool GameObject::IsColliding(const GameObject& other)
{
	// 크기가 너무 작다면 판정 크기를 3 x 3 으로 처리
	int width = sprite.sizeX > 3 ? sprite.sizeX : 3;
	int height = sprite.sizeY > 3 ? sprite.sizeY : 3;

	int otherWidth = other.sprite.sizeX > 3 ? other.sprite.sizeX : 3;
	int otherHeight = other.sprite.sizeY > 3 ? other.sprite.sizeY : 3;

	// AABB 충돌 검사
	return x < other.x + otherWidth &&
		   x + width > other.x &&
		   y < other.y + otherHeight &&
		   y + height > other.y;
}

void GameObject::OnCollision(GameObject& other)
{

}

void GameObject::Destroy()
{
	
}