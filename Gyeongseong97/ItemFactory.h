#pragma once
#include "Enums.h"

struct ItemInfo
{
	SpawnType type;
	int x;
	int y;
	float speed = 0.5f;
};

class ItemFactory
{
public:
	static void CreateItem(ItemInfo item);
};

