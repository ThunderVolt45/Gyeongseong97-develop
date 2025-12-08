#pragma once
#include "ImageLoader.h"
#include "GameConstants.h"
#include <memory>

class GameObject
{
private:
	bool active = true; // 활성화/비활성화 상태

public:
	float x = 0;
	float y = 0;
	Sprite sprite;
	TargetLayer layer = TargetLayer::Main;

	GameObject();
	GameObject(int x, int y);
	GameObject(int x, int y, int w, int h, std::wstring spriteName);
	~GameObject();

	/// <summary>
	/// GameObject의 중심 좌표의 X값을 반환하는 함수
	/// </summary>
	/// <returns>GameObject의 중심 좌표의 X값</returns>
	float GetCenterX();

	/// <summary>
	/// GameObject의 중심 좌표의 Y값을 반환하는 함수
	/// </summary>
	/// <returns>GameObject의 중심 좌표의 Y값</returns>
	float GetCenterY();

	/// <summary>
	/// 매 Logic Frame 마다 호출되는 메서드
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 객체가 파괴될 때 호출되는 메서드
	/// </summary>
	/// <param name="self">자기 자신의 shared_ptr</param>
	virtual void OnDestroy(std::shared_ptr<GameObject> self);

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

	/// <summary>
	/// GameObject의 파괴를 알리는 메서드 (풀링 시스템에서 재활용을 위해 사용)
	/// </summary>
	virtual void Destroy(); // public으로 변경 및 virtual 유지
};


