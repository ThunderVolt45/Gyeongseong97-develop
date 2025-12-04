#pragma once
#include "BossState.h"

class BossStateAttackDive : public BossState
{
public:
	void Update(Narration& boss) override;
};
