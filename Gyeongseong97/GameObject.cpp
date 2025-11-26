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