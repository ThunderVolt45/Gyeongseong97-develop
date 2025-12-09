#pragma once
#include "BossState.h"

class BossStateSpawnArmy : public BossState
{
public:
    void Update(Narration& boss) override;
};

