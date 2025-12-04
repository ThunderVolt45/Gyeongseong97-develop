#pragma once
#include "BossState.h"

class BossStateAttackShot : public BossState
{
public:
	void Update(Narration& boss) override;
};
