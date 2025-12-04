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
			// 객체 파괴시 객체 스스로 수행해야 할 로직이 있다면 실행한다
			obj->OnDestroy(obj);

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
