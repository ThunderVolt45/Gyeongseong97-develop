#include "BossState.h"
#include "BossStateAppeared.h"
#include "BossStateAttackBomb.h"
#include "BossStateAttackDive.h"
#include "BossStateAttackShot.h"
#include "BossStateDeath.h"
#include "BossStateIdle.h"
#include "BossStateSpawnCarmikaze.h"
#include "BossStateSpawnVanguard.h"

// 정적 인스턴스 정의
BossStateAppeared BossState::appeared;
BossStateAttackBomb BossState::bomb;
BossStateAttackDive BossState::dive;
BossStateAttackShot BossState::shot;
BossStateDeath BossState::death;
BossStateIdle BossState::idle;
BossStateSpawnCarmikaze BossState::carmikaze;
BossStateSpawnVanguard BossState::vanguard;
