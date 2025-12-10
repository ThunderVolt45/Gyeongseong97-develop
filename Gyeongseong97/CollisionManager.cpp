#include "CollisionManager.h"

#include "GameConstants.h"


void CollisionManager::ProcessCollisions(std::list<std::shared_ptr<GameObject>>& gameObjects, Player& player, std::set<GameObject*>& objectsToDestroy)
{
	// Grid 초기화
	// 각 셀은 해당 영역에 있는 오브젝트들의 포인터 목록을 갖는다
	std::vector<GameObject*> grid[GRID_ROWS][GRID_COLS];

	// 중복 충돌 처리를 방지하기 위한 집합
	std::set<std::pair<GameObject*, GameObject*>> processedPairs;

	// Grid 등록
	// 모든 게임 오브젝트를 순회하며 그리드에 등록한다.
	for (const auto& objPtr : gameObjects)
	{
		GameObject* obj = objPtr.get();

		// 이미 파괴 예정인 객체는 충돌 처리에서 제외
		if (objectsToDestroy.count(obj)) continue;

		// 객체가 차지하는 그리드 셀 범위 계산
		int w = obj->sprite.sizeX > 0 ? obj->sprite.sizeX : 1;
		int h = obj->sprite.sizeY > 0 ? obj->sprite.sizeY : 1;

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
				grid[r][c].push_back(obj);
			}
		}
	}

	// 충돌 검사 시작

	// (A) 플레이어 충돌 검사
	// 플레이어가 위치한 그리드 셀만 검사
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

				// 중복 체크
				GameObject* pPtr = &player;
				auto key = std::make_pair(std::min(pPtr, other), std::max(pPtr, other));
				if (processedPairs.count(key)) continue;

				if (player.IsColliding(*other))
				{
					processedPairs.insert(key);
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

					// 중복 체크
					auto key = std::make_pair(std::min(objA, objB), std::max(objA, objB));
					if (processedPairs.count(key)) continue;

					// 충돌 검사
					if (objA->IsColliding(*objB))
					{
						processedPairs.insert(key);
						objA->OnCollision(*objB);
						objB->OnCollision(*objA);
					}
				}
			}
		}
	}
}
