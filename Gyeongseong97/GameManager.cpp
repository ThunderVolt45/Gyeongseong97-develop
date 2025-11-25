#pragma execution_character_set( "utf-8" )

#include "GameManager.h"

#define NOMINMAX // Prevent min/max macro conflicts with windows.h
#include <windows.h>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/component/component.hpp>

// 게임 설정 상수
const int GAME_WIDTH = 160;
const int GAME_HEIGHT = 120; // 캔버스 높이는 텍스트 높이의 2배 (Block 기준)

GameManager::GameManager()
{
	player = GameObject();

	// 플레이어 위치 초기화
	player.x = GAME_WIDTH / 2;
	player.y = GAME_HEIGHT / 2 - 10;

	IsRunning = true;
}

GameManager::~GameManager()
{

}

void GameManager::Reset()
{
	// 플레이어 위치 초기화
	player.x = GAME_WIDTH / 2;
	player.y = GAME_HEIGHT / 2 - 10;

	bullets.clear();
	enemies.clear();
	score = 0;
	isGameOver = false;
	tick = 0;
}

void GameManager::Update()
{
	// 게임 오버 상태에선 중단
	if (isGameOver)
	{
		return;
	}

	tick++;

	// 사격 쿨다운
	if (shootCooldown > 0) shootCooldown--;

	// GetAsyncKeyState를 사용하여 키 상태를 직접 확인 (동시 입력 및 부드러운 이동 지원)
	if ((GetAsyncKeyState(VK_LEFT) & 0x8000) && player.x > 2) player.x -= 2;
	if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) && player.x < GAME_WIDTH - 2) player.x += 2;
	if ((GetAsyncKeyState(VK_UP) & 0x8000) && player.y > 2) player.y -= 1;
	if ((GetAsyncKeyState(VK_DOWN) & 0x8000) && player.y < GAME_HEIGHT - 2) player.y += 1;

	// 발사 (Z 키)
	if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && shootCooldown <= 0)
	{
		GameObject bullet = GameObject(player.x, player.y - 3);
		bullets.push_back(bullet);
		shootCooldown = 5; // 5틱(약 100ms) 쿨다운
	}
	
	// 총알 이동
	for (GameObject& bullet : bullets)
	{
		bullet.y -= 2;
	}

	// 화면 밖으로 나간 총알 제거
	bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const GameObject& obj) { return obj.y < 0; }), bullets.end());

	// 일정 간격으로 적 생성
	if (tick % 50 == 0)
	{
		int randomX = std::rand() % (GAME_WIDTH - 4) + 2;
		GameObject enemy = GameObject(randomX, 0);
		enemies.push_back(enemy);
	}

	// 적 이동
	for (GameObject& enemy : enemies)
	{
		enemy.y += tick % 2 == 0 ? 1 : 0;
	}

	// 충돌 판정
	// 총알 -> 적
	for (auto bullet = bullets.begin(); bullet != bullets.end();)
	{
		bool hit = false;

		for (auto enemy = enemies.begin(); enemy != enemies.end();)
		{
			// 거리 기반 충돌 체크
			if (std::abs(bullet->x - enemy->x) < 3 && std::abs(bullet->y - enemy->y) < 3)
			{
				enemies.erase(enemy);
				
				hit = true;
				score += 100;
				break;
			}
			else
			{
				enemy++;
			}
		}

		if (hit)
		{
			bullet = bullets.erase(bullet);
		}
		else
		{
			bullet++;
		}
	}

	// 적 -> 플레이어
	for (auto enemy : enemies)
	{
		if (std::abs(enemy.x - player.x) < 3 && std::abs(enemy.y - player.y) < 3)
		{
			isGameOver = true;
		}
	}
}

ftxui::Element GameManager::Render()
{
	// 캔버스 생성
	auto canvas = ftxui::Canvas(GAME_WIDTH, GAME_HEIGHT);

	// 플레이어 그리기 (초록색 ㅗ)
	canvas.DrawBlock(player.x, player.y - 1, true, ftxui::Color::Green);
	canvas.DrawBlock(player.x - 1, player.y, true, ftxui::Color::Green);
	canvas.DrawBlock(player.x, player.y, true, ftxui::Color::Green);
	canvas.DrawBlock(player.x + 1, player.y, true, ftxui::Color::Green);

	// 총알 그리기
	for (GameObject bullet : bullets)
	{
		canvas.DrawBlock(bullet.x, bullet.y, true, ftxui::Color::Yellow);
	}

	// 적 그리기
	for (GameObject enemy : enemies)
	{
		canvas.DrawBlock(enemy.x, enemy.y, true, ftxui::Color::Red);
		canvas.DrawBlock(enemy.x + 1, enemy.y, true, ftxui::Color::Red);
		canvas.DrawBlock(enemy.x, enemy.y + 1, true, ftxui::Color::Red);
		canvas.DrawBlock(enemy.x + 1, enemy.y + 1, true, ftxui::Color::Red);
	}

	// UI 구성
	auto UI = ftxui::canvas(std::move(canvas)) | ftxui::border | ftxui::center;

	std::string uiText = "Score : " + std::to_string(score);
	if (isGameOver) uiText += " | GAME OVER! (R키를 눌러 재시작, Q키를 눌러 나가기)";

	// 렌더링 타겟 반환
	return ftxui::window
	(
		ftxui::text("Gyeongseong 97"),
		ftxui::hbox(
			{
				UI,
				ftxui::vbox(
					{
						ftxui::text(uiText) | ftxui::border | ftxui::bold
					}
				) | ftxui::center
			}
		)
	);
}

bool GameManager::OnEvent(ftxui::Event event)
{
	// 화면 갱신 이벤트는 무시한다
	if (event == ftxui::Event::Custom)
	{
		return true;
	}

	// 나가기 (Q, Escape)
	if (event == ftxui::Event::Character('q') || event == ftxui::Event::Escape)
	{
		IsRunning = false;
		return true;
	}

	// 재시작 (R)
	if (isGameOver)
	{
		if (event == ftxui::Event::Character('r'))
		{
			Reset();
		}

		return true;
	}

	return true;
}
