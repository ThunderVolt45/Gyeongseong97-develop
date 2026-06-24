# 게임 오브젝트 (Game Objects)

화면에 존재하는 모든 개체의 기반 클래스 `GameObject`와 그 상속 계층입니다.

- **`GameObject`**: 모든 게임 개체의 기반 클래스. 위치·스프라이트·충돌·생명주기(`Update`/`OnDestroy`/`Destroy`)를 정의.
- **`Player`**: 김두한. 무기 3종(현재/기본/수류탄)을 보유하고 입력에 따라 이동·사격.
- **`Enemy`**: 적의 기반 클래스. 체력·속도·점수 및 피격 이펙트 처리. `Army`, `Vanguard`, `Carmikaze`, 보스 `Narration`이 상속.
- **`Bullet`**: 총알. 콜백(`onUpdate`/`onDestroy`)으로 유도탄·폭발 등 특수 동작 부여 가능. (풀링 대상)
- **`Explosion`**: 폭발 이펙트 + 범위 피해. (풀링 대상)
- **`WeaponItem`**: 떨어지는 무기 아이템. 획득 시 플레이어 무기 교체.

> 보스 `Narration`의 상태 패턴 상세는 [boss-state.md](boss-state.md), 무기 시스템은 [weapons.md](weapons.md) 참고.

```mermaid
classDiagram
    direction TB

    class GameObject {
        - bool active
        + ObjectType type
        + TargetLayer layer
        + float x
        + float y
        + Sprite sprite
        + GetCenterX() float
        + GetCenterY() float
        + IsOutOfScreen() bool
        + IsColliding(other) bool
        + IsActive() bool
        + SetActive(isActive) void
        + Update() void
        + OnDestroy(self) void
        + OnCollision(other) void
        + Destroy() void
    }

    class Player {
        - Sprite defaultSprite
        - Sprite deathSprite
        - int cooldown
        + Weapon currentWeapon
        + Weapon defaultWeapon
        + Weapon grenadeWeapon
        + float maxHealth
        + float health
        + bool invincible
        + Reset() void
        + Update() void
        + OnCollision(other) void
        + TakeDamage(damage) void
        + Death() void
        + Shoot() void
        + Grenade() void
        + EquipWeapon(newWeapon) void
    }

    class Enemy {
        # Sprite originalSprite
        # Sprite hitSprite
        # int hitEffectTick
        + float health
        + float speed
        + int killScore
        # ProcessHitEffect() void
        + TakeDamage(damage) void
        + Update() void
        + OnCollision(other) void
        + Destroy() void
    }

    class Army {
        - int cooldown
        - Attack() void
        + Update() void
    }

    class Vanguard {
        - int cooldown
        - int stunTime
        - Tracking() void
        - Attack() void
        + Update() void
    }

    class Carmikaze {
        - int direction
        - Destroy() void
        + Update() void
    }

    class Narration {
        + BossState state
        + BossStateEnum lastBossState
        + BossStateEnum bossState
        + bool invincible
        + int defaultPosX
        + int defaultPosY
        + long long internalTick
        + int internalCounter
        + TakeDamage(damage) void
        + Update() void
        + OnCollision(other) void
        + ChangePattern() void
        + Destroy() void
    }

    class Bullet {
        + int damage
        + int lifeTick
        + float speedX
        + float speedY
        + bool isPlayer
        + function onUpdate
        + function onDestroy
        + Reset(x, y, speedX, speedY, isMine, damage, lifeTick) void
        + SetCustomBehavior(onUpdate, onDestroy) void
        + SetCustomBehavior(sprite, onUpdate, onDestroy) void
        + Update() void
        + Destroy() void
        + GetDamage() int
        + OnDestroy(self) void
    }

    class Explosion {
        - vector~Sprite~ commonSprites$
        - vector~Sprite~ customSprites
        - int lifeTimeTick
        - int tick
        - int animationIndex
        - bool isCommonSize
        - int damage
        - bool isPlayer
        - SetSprites(w, h) void
        + Reset(x, y, w, h, damage, isPlayer) void
        + Update() void
        + OnCollision(other) void
        + OnDestroy(self) void
        + LoadSprites(sprites, w, h) void$
    }

    class WeaponItem {
        + float speed
        + WeaponType weaponType
        + WeaponItem(x, y, speed, type)
        + Update() void
        + OnCollision(other) void
    }

    GameObject <|-- Player
    GameObject <|-- Enemy
    GameObject <|-- Bullet
    GameObject <|-- Explosion
    GameObject <|-- WeaponItem

    Enemy <|-- Army
    Enemy <|-- Vanguard
    Enemy <|-- Carmikaze
    Enemy <|-- Narration

    Player ..> InputManager : IsKeyDown
    GameObject ..> ImageLoader : CreateSprite
    GameObject ..> AudioManager : PlayAudio
```
