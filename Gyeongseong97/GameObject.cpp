#include "GameObject.h"
#include "ImageLoader.h"

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
	this->x = x - w / 2;
	this->y = y - h / 2;

	sprite = ImageLoader::CreateSpriteFromImage(spriteName, w, h);
}

GameObject::~GameObject()
{

}

void GameObject::Update()
{

}

bool GameObject::IsColliding(const GameObject& other)
{
	// 스프라이트 크기가 유효하지 않으면 3x3로 처리
	int width = sprite.sizeX > 0 ? sprite.sizeX : 3;
	int height = sprite.sizeY > 0 ? sprite.sizeY : 3;

	int otherWidth = other.sprite.sizeX > 0 ? other.sprite.sizeX : 3;
	int otherHeight = other.sprite.sizeY > 0 ? other.sprite.sizeY : 3;

	// AABB 충돌 검사
	return x < other.x + otherWidth &&
		   x + width > other.x &&
		   y < other.y + otherHeight &&
		   y + height > other.y;
}