#pragma once
#include "BossState.h"

class BossStateSpawnVanguard : public BossState
{
public:
	void Update(Narration& boss) override;
};
