#pragma once
#include <vector>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "GameObject.h"

class GameManager
{
private:
	GameObject player;
	std::vector<GameObject> bullets;
	std::vector<GameObject> enemies;
	
	std::vector<GameObject> gameObjects;

	int score = 0;
	long long tick = 0;
	int shootCooldown = 0;
	bool isGameOver = false;

	void Reset();
	void DrawObjectSprite(ftxui::Canvas& canvas, GameObject object);

public:
	std::atomic<bool> IsRunning;

	GameManager();
	~GameManager();

	void Update();
	ftxui::Element Render();
	bool OnEvent(ftxui::Event event);
};

