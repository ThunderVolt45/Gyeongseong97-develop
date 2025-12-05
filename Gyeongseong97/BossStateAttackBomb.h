#pragma once
#include "BossState.h"

class BossStateAttackBomb : public BossState
{
public:
	void Update(Narration& boss) override;
};

