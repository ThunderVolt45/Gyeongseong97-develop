#pragma execution_character_set( "utf-8" )
#include "GameManager.h"

#include "AudioManager.h"
#include "StageManager.h"
#include "GameConstants.h"
#include "Enums.h"
#include "CollisionManager.h"
#include "InputManager.h"

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/component/component.hpp>

#include <set>
#include <algorithm>

GameManager::GameManager()
{

}

GameManager::~GameManager()
{

}

void GameManager::Initialize()
{
	// 플레이어 생성
	player = Player(PLAYER_DEFAULT_POSITION_X, PLAYER_DEFAULT_POSITION_Y);

	// 스테이지 초기화
	stageManager.Initialize();
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

	// 변수 리셋
	score = 0;
	IsGameOver = false;
	IsGameClear = false;
	IsGamePause = false;
	tick = 0;

	// 게임 시작
	IsRunning = true;
}

void GameManager::UpdateGameObjects(std::list<std::shared_ptr<GameObject>> objects)
{
	// 게임 오브젝트 업데이트
	for (auto iter = objects.begin(); iter != objects.end(); )
	{
		auto& obj = *iter;

		// 객체별 로직 업데이트
		obj->Update();

		// 화면 밖으로 나갔다면 즉시 제거 목록에 추가하고 업데이트 중단
		if (obj->IsOutOfScreen())
		{
			// 파괴 예정 명단에 추가한다
			DestroyGameObject(obj.get());
		}

		iter++;
	}
}

void GameManager::EraseGameObjects()
{
	// 제거 명단에 오른 오브젝트가 없다면 중단
	if (objectsToDestroy.empty())
	{
		return;
	}

	// 게임 오브젝트 삭제!
	gameObjects.erase(
		std::remove_if(gameObjects.begin(), gameObjects.end(),
			[&](const std::shared_ptr<GameObject>& ptr) {
				if (objectsToDestroy.count(ptr.get()) > 0)
				{
					// 객체 파괴시 객체 스스로 수행해야 할 로직이 있다면 실행한다
					ptr->OnDestroy(ptr);

					return true;
				}

				return false;
			}
		),
		gameObjects.end()
	);

	// 제거 명단 정리
	objectsToDestroy.clear();
}

void GameManager::CreateGameObject(std::shared_ptr<GameObject> gameObject, TargetLayer layer)
{
	std::lock_guard<std::recursive_mutex> lock(gameMutex);
	gameObject->layer = layer;
	gameObjects.push_back(gameObject);
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
		
		// Enemy 타입이라면 모조리 파괴
		if (ptr->type == ObjectType::Enemy)
		{
			ptr->Destroy();
		}
	}
}

bool GameManager::IsEnemyAlive()
{
	for (const auto& object : gameObjects)
	{
		if (object.get()->type == ObjectType::Enemy)
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
		if (object.get()->type == ObjectType::Enemy)
		{
			count++;
		}
	}

	return count;
}

void GameManager::Update()
{
	// 일시 정지 상태에선 아무 것도 하지 않는다
	if (IsGamePause) return;

	// InputManager 업데이트 (키 상태 갱신)
	InputManager::GetInstance().Update();

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
	UpdateGameObjects(gameObjects);

	// 충돌 검사 (CollisionManager 위임)
	CollisionManager::ProcessCollisions(gameObjects, player, objectsToDestroy);

	// 제거 명단에 오른 오브젝트를 모두 제거
	EraseGameObjects();

	// 게임 오버 상태가 되면 여기서 중단
	if (IsGameOver || IsGameClear) return;

	// 스테이지 갱신
	stageManager.Update();
}

bool GameManager::OnEvent(ftxui::ScreenInteractive& screen, ftxui::Event event)
{
	// 화면 갱신 이벤트는 무시한다
	if (event == ftxui::Event::Custom)
	{
		return true;
	}

	// 일시정지 (Escape)
	if (event == ftxui::Event::Escape)
	{
		if (IsGameOver || IsGameClear)
		{
			return false;
		}

		IsGamePause = !IsGamePause;

		return true;
	}

	// 나가기 (Q)
	if (event == ftxui::Event::Q || event == ftxui::Event::q)
	{
		if (!IsGameOver && !IsGameClear && !IsGamePause)
		{
			return false;
		}

		// 나갈 땐 뒷정리를 해야한다
		Reset();
		IsRunning = false;
		
		screen.Exit();

		return true;
	}

	// 재시작 (R)
	if (event == ftxui::Event::R || event == ftxui::Event::r)
	{
		if (!IsGameOver && !IsGameClear && !IsGamePause)
		{
			return false;
		}

		Reset();

		return true;
	}

	return true;
}
