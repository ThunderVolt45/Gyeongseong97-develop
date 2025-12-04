#pragma once
#include "BossState.h"

class BossStateIdle : public BossState
{
public:
	void Update(Narration& boss) override;
};
