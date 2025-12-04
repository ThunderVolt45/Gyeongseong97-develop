#pragma once
#include "BossState.h"

class BossStateDeath : public BossState
{
public:
	void Update(Narration& boss) override;
};
