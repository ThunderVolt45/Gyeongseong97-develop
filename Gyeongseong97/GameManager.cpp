#pragma execution_character_set( "utf-8" )
#include "GameManager.h"
#include "AudioManager.h"
#include "StageManager.h"
#include "GameConstants.h"

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
	stage.Initialize();

	// 플레이어 생성
	player = Player(PLAYER_DEFAULT_POSITION_X, PLAYER_DEFAULT_POSITION_Y);

	// 백그라운드 생성
	// 배경은 없는 게 성능 상 나은 듯 ㅈㅈ
	//background = GameObject(GAME_WIDTH * 0.5f, GAME_HEIGHT * 0.5f, GAME_WIDTH, GAME_HEIGHT, L"background.png");

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
	stage.Reset();

	score = 0;
	IsGameOver = false;
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

	// 반복문 도중 벡터 수정으로 인한 충돌을 막기 위해 대기열에 넣음
	// objectsToCreate.push_back(gameObject);

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

void GameManager::Update()
{
	// Update와 Render가 동시에 호출되어 벌어지는 참사를 막기 위해 mutex로 잠가버린다.
	// std::lock_guard를 쓰면 lock_guard가 소멸될 때 자동으로 mutex가 해제된다.
	std::lock_guard<std::recursive_mutex> lock(gameMutex);

	// 틱
	tick++;

	// 대기열에 있는 오브젝트들을 실제 게임 오브젝트 리스트에 추가
	/*if (!objectsToCreate.empty())
	{
		gameObjects.insert(gameObjects.end(), objectsToCreate.begin(), objectsToCreate.end());
		objectsToCreate.clear();
	}*/
	
	// 플레이어 업데이트
	player.Update();

	// 플레이어의 체력이 다 떨어졌다면 게임 오버
	if (player.health <= 0)
	{
		IsGameOver = true;
	}

	// Grid 초기화
	// 각 셀은 해당 영역에 있는 오브젝트들의 포인터 목록을 갖는다
	std::vector<GameObject*> grid[GRID_ROWS][GRID_COLS];

	// 게임 오브젝트 업데이트 및 그리드 등록 (Broad Phase)
	for (auto iter = gameObjects.begin(); iter != gameObjects.end(); )
	{
		auto& obj = *iter;
		
		// 객체별 로직 업데이트
		obj->Update();

		// 화면 밖으로 나갔다면 즉시 제거 목록에 추가하고 업데이트 중단
		if (obj->IsOutOfScreen())
		{
			iter = gameObjects.erase(iter);
			continue;
		}

		// 객체가 차지하는 그리드 셀 범위 계산
		// 객체의 크기(Sprite Size)에 따라 여러 셀에 걸칠 수 있음을 고려
		int w = obj->sprite.sizeX > 0 ? obj->sprite.sizeX : 1;
		int h = obj->sprite.sizeY > 0 ? obj->sprite.sizeY : 1;

		// std::max(0, ...) -> 값 < 0 ? 0 : 값
		// std::min(GRID_COLS - 1, ...) -> 값 > GRID_COLS - 1 ? GRID_COLS - 1 : 값
		int tempVal;

		tempVal = obj->x / CELL_SIZE;
		int startCol = tempVal < 0 ? 0 : tempVal;

		tempVal = (obj->x + w) / CELL_SIZE;
		int endCol = tempVal > GRID_COLS - 1 ? GRID_COLS - 1 : tempVal;

		tempVal = obj->y / CELL_SIZE;
		int startRow = tempVal < 0 ? 0 : tempVal;

		tempVal = (obj->y + h) / CELL_SIZE;
		int endRow = tempVal > GRID_ROWS - 1 ? GRID_ROWS - 1 : tempVal;

		// 해당 범위의 모든 셀에 객체 포인터 등록
		for (int r = startRow; r <= endRow; ++r)
		{
			for (int c = startCol; c <= endCol; ++c)
			{
				grid[r][c].push_back(obj.get());
			}
		}

		iter++;
	}

	// 충돌 검사
	
	// (A) 플레이어 충돌 검사
	// 플레이어가 위치한 그리드 셀만 검사하면 됩니다.
	int tempVal;

	tempVal = player.x / CELL_SIZE;
	int pStartCol = tempVal < 0 ? 0 : tempVal;

	tempVal = (player.x + player.sprite.sizeX) / CELL_SIZE;
	int pEndCol = tempVal > GRID_COLS - 1 ? GRID_COLS - 1 : tempVal;

	tempVal = player.y / CELL_SIZE;
	int pStartRow = tempVal < 0 ? 0 : tempVal;

	tempVal = (player.y + player.sprite.sizeY) / CELL_SIZE;
	int pEndRow = tempVal > GRID_ROWS - 1 ? GRID_ROWS - 1 : tempVal;

	for (int r = pStartRow; r <= pEndRow; ++r)
	{
		for (int c = pStartCol; c <= pEndCol; ++c)
		{
			for (GameObject* other : grid[r][c])
			{
				// 이미 죽은 객체는 무시
				if (objectsToDestroy.count(other)) continue;

				if (player.IsColliding(*other))
				{
					player.OnCollision(*other);
					other->OnCollision(player);
				}
			}
		}
	}

	// (B) 오브젝트 간 충돌 검사 (총알 vs 적 등)
	for (int r = 0; r < GRID_ROWS; ++r)
	{
		for (int c = 0; c < GRID_COLS; ++c)
		{
			auto& cell = grid[r][c];
			if (cell.size() < 2) continue; // 2개 이상 있어야 충돌 가능

			for (size_t i = 0; i < cell.size(); ++i)
			{
				for (size_t j = i + 1; j < cell.size(); ++j)
				{
					GameObject* objA = cell[i];
					GameObject* objB = cell[j];

					// 둘 중 하나라도 이미 죽은 상태면 건너뜀
					if (objectsToDestroy.count(objA) || objectsToDestroy.count(objB)) continue;

					// 충돌 검사
					if (objA->IsColliding(*objB))
					{
						objA->OnCollision(*objB);
						objB->OnCollision(*objA);
					}
				}
			}
		}
	}

	// 제거 명단에 오른 오브젝트를 모두 제거
	if (!objectsToDestroy.empty())
	{
		gameObjects.erase(
			std::remove_if(gameObjects.begin(), gameObjects.end(),
				[&](const std::shared_ptr<GameObject>& ptr) {
					return objectsToDestroy.count(ptr.get()) > 0;
				}
			),
			gameObjects.end()
		);
		objectsToDestroy.clear();
	}

	// 게임 오버 상태가 되면 여기서 중단
	if (IsGameOver) return;

	// 스테이지 갱신
	stage.Update();
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
