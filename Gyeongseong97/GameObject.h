#pragma once
#include "ImageLoader.h"

class GameObject
{
private:
	/// <summary>
	/// 폭★8
	/// </summary>
	virtual void Destroy();

public:
	float x = 0;
	float y = 0;
	Sprite sprite;

	GameObject();
	GameObject(int x, int y);
	GameObject(int x, int y, int w, int h, std::wstring spriteName);
	~GameObject();

	/// <summary>
	/// 매 Logic Frame 마다 호출되는 메서드
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 자기 자신이 Screen 바깥으로 나갔는 지 검사하는 메소드
	/// </summary>
	bool IsOutOfScreen();
	
	/// <summary>
	/// 다른 GameObject와 충돌 여부를 검사하는 메서드
	/// </summary>
	/// <param name="other">다른 GameObject</param>
	/// <returns>충돌 여부</returns>
	bool IsColliding(const GameObject& other);

	/// <summary>
	/// 다른 GameObject와 충돌했을 때 호출할 메서드
	/// </summary>
	/// <param name="other">다른 GameObject</param>
	virtual void OnCollision(GameObject& other);
};

