#pragma once

enum class TargetLayer
{
	Background,
	Main,
	Foreground
};

enum class ObjectType
{
	Default,
	Player,
	Enemy,
	Bullet
};

enum class SpawnPosition : int
{
	Random = -100, // 무작위로 결정
	PlayerPosition = -101, // 플레이어의 x / y 값을 가져옴
	Min = -102, // 최소 값
	Max = -103 // 최대 값
};

enum class SpawnType : int
{
	Instigated,
	Vanguard,
	Carmikaze,
	Narration,
	Army,
	ItemHMG = -1,
	ItemShotgun = -2,
	ItemGranade = -3,
	ItemRocket = -4
};

enum class WeaponType
{
	Default,
	HMG,
	Shotgun,
	Grenade,
	Rocket
};