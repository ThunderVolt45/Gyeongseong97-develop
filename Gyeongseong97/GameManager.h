#pragma once
#include <vector>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "GameObject.h"
#include "Player.h"
#include "Bullet.h"

class GameManager
{
private:
	/// <summary>
	/// GameManager의 생성자
	/// private으로 선언하여 외부에서 직접 개체를 생성하는 것을 방지한다.
	/// </summary>
	GameManager();

	/// <summary>
	/// GameManager의 소멸자.
	/// private으로 선언하여 C++ 런타임이 직접 소멸시키도록 한다.
	/// </summary>
	~GameManager();

	/// 복사 생성자와 대입 연산자를 삭제해 인스턴스가 복제되지 못하게 한다.
	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager&) = delete;

	Player player;
	std::vector<Bullet> bullets;
	std::vector<GameObject> enemies;
	
	std::vector<GameObject> gameObjects;

	int score = 0;
	long long tick = 0;
	int shootCooldown = 0;
	bool isGameOver = false;

	void Reset();
	void DrawObjectSprite(ftxui::Canvas& canvas, GameObject object);

public:
	/// <summary>
	/// GameManager의 유일한 인스턴스에 접근하는 메서드
	/// </summary>
	/// <returns>GameManager 객체의 참조값</returns>
	static GameManager& GetInstance();

	std::atomic<bool> IsRunning;

	void Update();
	ftxui::Element Render();
	bool OnEvent(ftxui::Event event);

	void CreateBullet(Bullet bullet);
};

