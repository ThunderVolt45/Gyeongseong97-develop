#pragma execution_character_set( "utf-8" )
#include "GameManager.h"

#define NOMINMAX // Prevent min/max macro conflicts with windows.h
#include <windows.h>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/component/component.hpp>

// 게임 설정 상수
extern const int GAME_WIDTH;
extern const int GAME_HEIGHT;
const int PLAYER_DEFAULT_POSITION_X = GAME_WIDTH / 2;
const int PLAYER_DEFAULT_POSITION_Y = GAME_HEIGHT - 20;

GameManager::GameManager()
{
	// 플레이어 초기화
	player = Player(PLAYER_DEFAULT_POSITION_X, PLAYER_DEFAULT_POSITION_Y, 40, 30, L"image.png");

	IsRunning = true;
}

GameManager::~GameManager()
{

}

GameManager& GameManager::GetInstance()
{
	static GameManager instance;
	return instance;
}

void GameManager::Reset()
{
	// 플레이어 초기화
	player.Reset();
	player.x = PLAYER_DEFAULT_POSITION_X - player.sprite.sizeX / 2;
	player.y = PLAYER_DEFAULT_POSITION_Y - player.sprite.sizeY / 2;

	bullets.clear();
	enemies.clear();
	score = 0;
	IsGameOver = false;
	tick = 0;
}

void GameManager::DrawObjectSprite(ftxui::Canvas& canvas, GameObject object)
{
	int spriteIndex = 0;
	for (int y = object.y; y < object.y + object.sprite.sizeY; y++)
	{
		// Sprite의 위가 잘리는 경우
		if (y < 0)
		{
			spriteIndex += object.sprite.sizeX;
			continue;
		}

		// Sprite의 아래가 잘리는 경우
		if (y >= GAME_HEIGHT)
		{
			break;
		}

		for (int x = object.x; x < object.x + object.sprite.sizeX; x++)
		{
			// Sprite의 왼쪽이 잘리는 경우
			if (x < 0)
			{
				spriteIndex++;
				continue;
			}

			// Sprite의 오른쪽이 잘리는 경우
			if (x >= GAME_WIDTH)
			{
				spriteIndex++;
				continue;
			}

			// index 값이 vector 크기를 넘을 경우 중단
			if (spriteIndex > object.sprite.colors.size() - 1)
			{
				break;
			}

			// Canvas에 색상으로 점(블록)을 찍는다.
			canvas.DrawBlock(x, y, true, object.sprite.colors[spriteIndex]);
			spriteIndex++;
		}
	}
}

void GameManager::CreateBullet(Bullet bullet)
{
	bullets.push_back(bullet);
}

void GameManager::Update()
{
	tick++;

	// 플레이어 업데이트
	player.Update();

	// 총알 업데이트
	for (GameObject& bullet : bullets)
	{
		bullet.Update();
	}

	// 화면 밖으로 나간 총알 제거
	bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const GameObject& obj) { return obj.y < 0; }), bullets.end());

	// 게임 오버 상태가 되면 여기서 중단한다
	if (IsGameOver)
	{
		return;
	}

	// 일정 간격으로 적 생성
	if (tick % 30 == 0)
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

	// Bullet 충돌 처리
	for (auto bullet = bullets.begin(); bullet != bullets.end();)
	{
		bool hit = false;

		// 플레이어 Bullet -> 적
		if (bullet->isMine)
		{
			for (auto enemy = enemies.begin(); enemy != enemies.end(); enemy++)
			{
				// 거리 기반 충돌 체크
				if (bullet->IsColliding(*enemy))
				{
					enemies.erase(enemy);
				
					hit = true;
					score += 100;
					break;
				}
			}
		}
		else // 적 Bullet -> 플레이어
		{
			if (bullet->IsColliding(player))
			{
				hit = true;
				player.health -= 1;

				if (player.health < 0)
				{
					IsGameOver = true;
				}
			}
		}

		// Bullet 명중시 제거
		if (hit)
		{
			bullet = bullets.erase(bullet);
		}
		else
		{
			bullet++;
		}
	}

	// 플레이어 - 적 충돌 처리
	for (auto enemy = enemies.begin(); enemy != enemies.end();)
	{
		if (enemy->IsColliding(player))
		{
			enemy = enemies.erase(enemy);
			player.health -= 1;

			if (player.health < 0)
			{
				IsGameOver = true;
			}
		}
		else
		{
			enemy++;
		}
	}
}

ftxui::Element GameManager::Render()
{
	// 캔버스 생성
	auto canvas = ftxui::Canvas(GAME_WIDTH, GAME_HEIGHT);

	// 플레이어 그리기
	DrawObjectSprite(canvas, player);

	// 총알 그리기
	for (Bullet bullet : bullets)
	{
		if (bullet.isMine)
		{
			canvas.DrawBlock(bullet.x, bullet.y, true, ftxui::Color::Yellow);
		}
		else
		{
			canvas.DrawBlock(bullet.x, bullet.y, true, ftxui::Color::HotPink);
		}
	}

	// 적 그리기
	for (GameObject enemy : enemies)
	{
		canvas.DrawBlock(enemy.x, enemy.y, true, ftxui::Color::Red);
		canvas.DrawBlock(enemy.x + 1, enemy.y, true, ftxui::Color::Red);
		canvas.DrawBlock(enemy.x, enemy.y + 1, true, ftxui::Color::Red);
		canvas.DrawBlock(enemy.x + 1, enemy.y + 1, true, ftxui::Color::Red);
	}

	// 캔버스를 박스로 감싸준다
	auto UI = ftxui::canvas(std::move(canvas)) | ftxui::border | ftxui::center;

	// 체력 바 그리기
	auto healthBar = ftxui::vbox({
			ftxui::text(L"Health : "),
			ftxui::gauge(player.health / player.maxHealth) | color(ftxui::Color::Red) | bgcolor(ftxui::Color::GrayDark)
		});

	// 기타 UI
	std::wstring textScore = L"Score : " + std::to_wstring(score);
	std::wstring textGameOver = IsGameOver ? L"GAME OVER!" : L"";
	std::wstring textRestart = IsGameOver ? L"R키를 눌러 재시작" : L"";
	std::wstring textQuit = IsGameOver ? L"Q키를 눌러 나가기" : L"";

	// 렌더링 타겟 반환
	return ftxui::hbox(
		{
			UI | ftxui::size(ftxui::WIDTH, ftxui::Constraint::EQUAL, GAME_WIDTH / 2),
			ftxui::vbox(
				{
					ftxui::text(textScore) | ftxui::border | ftxui::bold,
					ftxui::text(textGameOver) | ftxui::bold,
					ftxui::text(textRestart) | ftxui::bold,
					ftxui::text(textQuit) | ftxui::bold,
					healthBar
				}
			) | ftxui::size(ftxui::WIDTH, ftxui::Constraint::EQUAL, 20)
		}
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
	if (IsGameOver)
	{
		if (event == ftxui::Event::Character('r'))
		{
			Reset();
		}

		return true;
	}

	return true;
}
