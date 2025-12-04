#pragma once
#include "BossState.h"

class BossStateSpawnCarmikaze : public BossState
{
public:
	void Update(Narration& boss) override;
};
