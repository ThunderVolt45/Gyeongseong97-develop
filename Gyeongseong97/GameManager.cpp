#pragma execution_character_set( "utf-8" )
#include "GameManager.h"
#include "AudioManager.h"
#include "StageManager.h"
#include "GameConstants.h"
#include "Bullet.h"
#include "BulletPool.h"
#include "Explosion.h"
#include "ExplosionPool.h"
#include "CollisionManager.h"

#define NOMINMAX // Prevent min/max macro conflicts with windows.h
#define NODRAWTEXT
#include <windows.h>

#include <set>
#include <algorithm>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/component/component.hpp>

GameManager::GameManager()
{
	// 스테이지 초기화
	stageManager.Initialize();

	// 플레이어 생성
	player = Player(PLAYER_DEFAULT_POSITION_X, PLAYER_DEFAULT_POSITION_Y);

	// 백그라운드 생성
	// 배경은 없는 게 성능 상 나은 듯 ㅈㅈ
	background = GameObject(GAME_WIDTH * 0.5f, GAME_HEIGHT * 0.5f, GAME_WIDTH, GAME_HEIGHT, L"background.png");

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

	// 게임 오브젝트 초기화
	gameObjects.clear();
	objectsToDestroy.clear();
	objectsToCreate.clear();

	// 스테이지 초기화
	stageManager.Reset();

	if (onResetCallback)
	{
		onResetCallback();
	}

	// 사운드 초기화
	AudioManager& audioManager = AudioManager::GetInstance();
	audioManager.StopAudio(BGM_BOSS.data());
	audioManager.StopAudio(BGM_WIN.data());

	if (!audioManager.IsPlaying(BGM_MAIN.data()))
		audioManager.PlayAudio(BGM_MAIN.data(), BGM_VOULME, true);

	score = 0;
	IsGameOver = false;
	IsGameClear = false;
	tick = 0;
}

void GameManager::DrawObjectSprite(ftxui::Canvas& canvas, const GameObject& object)
{
	int spriteIndex = 0;

	// 오브젝트 좌표 값을 int로 반올림해 형변환
	int objX = static_cast<int>(std::round(object.x));
	int objY = static_cast<int>(std::round(object.y));

	// 만약 Sprite 정보가 없다면 흰색 블럭 하나를 찍는다
	if (object.sprite.sizeX == 0)
	{
		canvas.DrawBlock(objX, objY, true, ftxui::Color::White);
		canvas.DrawBlock(objX + 1, objY, true, ftxui::Color::White);
		canvas.DrawBlock(objX, objY + 1, true, ftxui::Color::White);
		canvas.DrawBlock(objX + 1, objY + 1, true, ftxui::Color::White);

		return;
	}

	// Sprite의 색상 값을 보고 Canvas에 블럭을 그린다.
	for (int y = objY; y < objY + object.sprite.sizeY; y++)
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

		for (int x = objX; x < objX + object.sprite.sizeX; x++)
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
			if (spriteIndex >= object.sprite.colors.size())
			{
				break;
			}

			// Canvas에 색상으로 점(블록)을 찍는다.
			ftxui::Color pixelColor = object.sprite.colors[spriteIndex];
			if (pixelColor.IsOpaque())
			{
				canvas.DrawBlock(x, y, true, pixelColor);
			}
			spriteIndex++;
		}
	}
}

void GameManager::CreateGameObject(std::shared_ptr<GameObject> gameObject, bool pushToBack)
{
	std::lock_guard<std::recursive_mutex> lock(gameMutex);

	if (pushToBack)
	{
		gameObjects.push_back(gameObject);
	}
	else
	{
		gameObjects.push_front(gameObject);
	}
}

void GameManager::DestroyGameObject(GameObject* gameObject)
{
	std::lock_guard<std::recursive_mutex> lock(gameMutex);
	objectsToDestroy.insert(gameObject);
}

void GameManager::DestroyAllEnemiesExcept(GameObject* except)
{
	std::lock_guard<std::recursive_mutex> lock(gameMutex);

	for (const auto& object : gameObjects)
	{
		GameObject* ptr = object.get();

		// 예외 대상이면 스킵
		if (ptr == except) continue;
		
		// Enemy 클래스(또는 파생 클래스)인지 확인
		auto enemy = dynamic_cast<Enemy*>(ptr);
		if (enemy != nullptr)
		{
			enemy->Destroy();
		}
	}
}

bool GameManager::IsEnemyAlive()
{
	for (const auto& object : gameObjects)
	{
		if (dynamic_cast<Enemy*>(object.get()) != nullptr)
		{
			// Enemy가 하나라도 발견되면 true 반환
			return true;
		}
	}

	return false;
}

int GameManager::GetEnemyCount()
{
	int count = 0;

	for (const auto& object : gameObjects)
	{
		if (dynamic_cast<Enemy*>(object.get()) != nullptr)
		{
			count++;
		}
	}

	return count;
}

void GameManager::Update()
{
	// Update와 Render가 동시에 호출되어 벌어지는 참사를 막기 위해 mutex로 잠가버린다.
	// std::lock_guard를 쓰면 lock_guard가 소멸될 때 자동으로 mutex가 해제된다.
	std::lock_guard<std::recursive_mutex> lock(gameMutex);

	// 틱
	tick++;
	
	// 플레이어 업데이트
	player.Update();

	// 플레이어의 체력이 다 떨어졌다면 게임 오버
	if (player.health <= 0)
	{
		IsGameOver = true;
	}

	// 게임 오브젝트 업데이트
	for (auto iter = gameObjects.begin(); iter != gameObjects.end(); )
	{
		auto& obj = *iter;
		
		// 객체별 로직 업데이트
		obj->Update();

		// 화면 밖으로 나갔다면 즉시 제거 목록에 추가하고 업데이트 중단
		if (obj->IsOutOfScreen())
		{
			// Bullet은 파괴하지 않고 반환한다
			if (auto bullet = std::dynamic_pointer_cast<Bullet>(obj))
			{
				BulletPool::GetInstance().ReturnBullet(bullet);
			}

			// Explosion 역시 파괴하지 않고 반환한다
			else if (auto explosion = std::dynamic_pointer_cast<Explosion>(obj))
			{
				ExplosionPool::GetInstance().ReturnExplosion(explosion);
			}

			iter = gameObjects.erase(iter);
			continue;
		}

		iter++;
	}

	// 충돌 검사 (CollisionManager 위임)
	CollisionManager::ProcessCollisions(gameObjects, player, objectsToDestroy);

	// 제거 명단에 오른 오브젝트를 모두 제거
	if (!objectsToDestroy.empty())
	{
		gameObjects.erase(
			std::remove_if(gameObjects.begin(), gameObjects.end(),
				[&](const std::shared_ptr<GameObject>& ptr) {
					if (objectsToDestroy.count(ptr.get()) > 0)
					{
						// Bullet 오브젝트는 제거하지 않고 오브젝트 풀로 반환
						if (auto bullet = std::dynamic_pointer_cast<Bullet>(ptr))
						{
							BulletPool::GetInstance().ReturnBullet(bullet);
						}

						// Explosion 오브젝트 역시 제거하지 않고 오브젝트 풀로 반환
						else if (auto explosion = std::dynamic_pointer_cast<Explosion>(ptr))
						{
							ExplosionPool::GetInstance().ReturnExplosion(explosion);
						}

						return true;
					}

					return false;
				}
			),
			gameObjects.end()
		);

		objectsToDestroy.clear();
	}

	// 게임 오버 상태가 되면 여기서 중단
	if (IsGameOver || IsGameClear) return;

	// 스테이지 갱신
	stageManager.Update();
}

ftxui::Element GameManager::Render()
{
	// 캔버스 생성
	auto canvas = ftxui::Canvas(GAME_WIDTH, GAME_HEIGHT);

	// 배경 그리기
	//DrawObjectSprite(canvas, background);
	

	// 임계 구역 설정
	{
		// Update와 Render가 동시에 호출되어 벌어지는 참사를 막기 위해 mutex로 잠가버린다.
		// std::lock_guard를 쓰면 lock_guard가 소멸될 때 자동으로 mutex가 해제된다.
		std::lock_guard<std::recursive_mutex> lock(gameMutex);

		// 플레이어 오브젝트 그리기
		DrawObjectSprite(canvas, player);

		// 게임 오브젝트 그리기
		for (const auto& object : gameObjects)
		{
			DrawObjectSprite(canvas, *object);
		}
	}

	// FPS 표시
	canvas.DrawText(0, 0, "FPS : " + std::to_string(currentFps) + ", Logic : " + std::to_string(currentLps));

	// 게임 오버 시 게임 오버 메시지 표시
	if (IsGameOver)
	{
		canvas.DrawText(GAME_WIDTH / 2 - (5 * 2) - 2, GAME_HEIGHT / 2 - 8, "GAME OVER!",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::Black;
				p.background_color = ftxui::Color::Red;
			});
		canvas.DrawText(GAME_WIDTH / 2 - (22 * 2) - 2, GAME_HEIGHT / 2, "김두한은 오렌지병이었던 고혈압으로 쓰러졌다.",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::White;
			});
		canvas.DrawText(GAME_WIDTH / 2 - (18 * 2) - 2, GAME_HEIGHT / 2 + 8, "(R키를 눌러 재시작, Q키를 눌러 나가기)",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::White;
			});
	}
	else if (IsGameClear)
	{
		canvas.DrawText(GAME_WIDTH / 2 - (4 * 2) - 2, GAME_HEIGHT / 2 - 8, "YOU WIN!",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::Black;
				p.background_color = ftxui::Color::Green;
			});
		canvas.DrawText(GAME_WIDTH / 2 - (25 * 2) - 2, GAME_HEIGHT / 2, "김두한은 사악한 공산당원들을 모조리 다 쓸어버렸다!",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::White;
			});
		canvas.DrawText(GAME_WIDTH / 2 - (18 * 2) - 2, GAME_HEIGHT / 2 + 8, "(R키를 눌러 재시작, Q키를 눌러 나가기)",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::White;
			});
	}

	// 캔버스를 박스로 감싸준다
	auto UI = ftxui::canvas(std::move(canvas)) | ftxui::border | ftxui::center;

	// 체력 바 그리기
	auto healthBar = ftxui::vbox({
			ftxui::text(L"Health : "),
			ftxui::gauge(player.health / player.maxHealth) | color(ftxui::Color::Red) | bgcolor(ftxui::Color::GrayDark)
		});

	// 기타 UI
	std::wstring textTime = L"Time : " + gameTime;
	std::wstring textScore = L"Score : " + std::to_wstring(score);

	// 렌더링 타겟 반환
	return ftxui::hbox(
		{
			UI | ftxui::size(ftxui::WIDTH, ftxui::Constraint::EQUAL, GAME_WIDTH / 2),
			ftxui::vbox(
				{
					ftxui::text(textTime) | ftxui::border | ftxui::bold,
					ftxui::text(textScore) | ftxui::border | ftxui::bold,
					ftxui::filler(),
					healthBar | ftxui::border
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
	if (IsGameOver || IsGameClear)
	{
		if (event == ftxui::Event::Character('r'))
		{
			Reset();
		}

		return true;
	}

	return true;
}
