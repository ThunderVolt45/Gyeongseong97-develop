#pragma once
#include "GameObject.h"
#include "Player.h"

#include <vector>
#include <list>
#include <memory>
#include <set>

class CollisionManager
{
public:
	// 게임 오브젝트 리스트와 플레이어 정보를 받아 충돌을 처리하는 함수
	static void ProcessCollisions(std::list<std::shared_ptr<GameObject>>& gameObjects, Player& player, std::set<GameObject*>& objectsToDestroy);
};