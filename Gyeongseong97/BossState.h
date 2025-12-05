#pragma once

// 전방 선언 (순환 참조 방지)
class Narration;
class BossStateAppeared;
class BossStateAttackBomb;
class BossStateAttackDive;
class BossStateAttackShot;
class BossStateDeath;
class BossStateIdle;
class BossStateSpawnCarmikaze;
class BossStateSpawnVanguard;

class BossState
{
public:
	// 상태 전환을 위한 정적 인스턴스들 (싱글톤처럼 사용)
	static BossStateAppeared appeared;
	static BossStateAttackBomb bomb;
	static BossStateAttackDive dive;
	static BossStateAttackShot shot;
	static BossStateDeath death;
	static BossStateIdle idle;
	static BossStateSpawnCarmikaze carmikaze;
	static BossStateSpawnVanguard vanguard;

	virtual ~BossState() = default;
	virtual void Update(Narration& boss) = 0;
};
