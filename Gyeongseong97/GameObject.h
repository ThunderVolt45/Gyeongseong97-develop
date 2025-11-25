#pragma once
#include "ImageLoader.h"

class GameObject
{
public:
	int x = 0;
	int y = 0;
	Sprite sprite;

	GameObject();
	GameObject(int x, int y);
	GameObject(int x, int y, int w, int h, std::wstring spriteName);
	~GameObject();

	virtual void Update();
};

