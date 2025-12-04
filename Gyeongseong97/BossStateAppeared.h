#pragma once
#include "BossState.h"

class BossStateAppeared : public BossState
{
public:
	void Update(Narration& boss) override;
};
