# 지원 시스템 / 데이터 (Systems & Data)

스테이지 진행, 객체 풀링, 오디오, 이미지 로드, 팩토리, 그리고 게임 전반에서 쓰이는 구조체·열거형 모음입니다.

- **`StageManager`**: 웨이브 기반 스테이지 진행. `Wave`/`SpawnData`를 읽어 팩토리로 적·아이템을 스폰하고, 모두 처치 시 `GameClear()`.
- **`EnemyFactory` / `ItemFactory`**: 스폰 데이터를 받아 적·아이템 객체를 생성 (Factory).
- **`ObjectPool<T>` (Singleton)**: `Bullet`·`Explosion` 등 빈번히 생성/파괴되는 객체를 재활용.
- **`AudioManager` (Singleton)**: miniaudio 기반 사운드 재생/페이드. `PlayingSoundInfo`로 개별 사운드를 추적.
- **`ImageLoader`**: stb 기반 이미지 → `Sprite` 변환 및 캐싱 (정적 유틸).

```mermaid
classDiagram
    direction LR

    class StageManager {
        - vector~Wave~ waves
        - float tickTimeToFloat
        - long long tick
        - size_t currentWaveIndex
        - size_t currentEnemyIndex
        - float delayTimer
        - GameClear() void
        + Initialize() void
        + Reset() void
        + Update() void
    }

    class EnemyFactory {
        + CreateEnemy(enemy) void$
    }
    class ItemFactory {
        + CreateItem(item) void$
    }

    class ObjectPool~T~ {
        <<Singleton>>
        - vector~T~ pool
        - recursive_mutex poolMutex
        + GetInstance() ObjectPool$
        + Get(args) T
        + Return(obj) void
    }

    class AudioManager {
        <<Singleton>>
        - ma_engine audioEngine
        - list~PlayingSoundInfo*~ activeSounds
        - mutex soundMutex
        - PlayAudioThread(pSoundInfo) void
        + GetInstance() AudioManager$
        + PlayAudio(audioPath, volume, loop) void
        + SetMasterVolume(volume) void
        + StopAudio(audioPath) void
        + FadeOutAudio(audioPath, fadeOutTimeInMS) void
        + IsPlaying(audioPath) bool
    }

    class PlayingSoundInfo {
        + ma_sound pSound
        + wstring audioPath
        + bool isLooping
        + bool isFading
        + time_point fadeStartTime
        + float fadeDuration
        + PlayingSoundInfo(pSound, audioPath, isLooping)
        + Play() void
        + Stop() void
        + FadeOut(milliseconds) void
        + IsPlaying() bool
        + Update() void
    }

    class ImageLoader {
        - map spriteCache$
        + CreateSpriteFromImage(fileName, sizeX, sizeY) Sprite$
        + FlipSpriteX(spriteToFlip) Sprite$
        + CreateHitSprite(originalSprite) Sprite$
    }

    %% ---- Data structures ----
    class Sprite {
        <<struct>>
        + int sizeX
        + int sizeY
        + vector~Color~ colors
    }
    class SpawnData {
        <<struct>>
        + SpawnType type
        + int x
        + int y
        + int health
        + float speed
        + float nextEnemyDelay
    }
    class Wave {
        <<struct>>
        + vector~SpawnData~ enemies
        + float nextWaveDelay
    }
    class EnemyInfo {
        <<struct>>
        + SpawnType type
        + int x
        + int y
        + int health
        + float speed
        + int killScore
    }
    class ItemInfo {
        <<struct>>
        + SpawnType type
        + int x
        + int y
        + float speed
    }

    %% ---- Enums ----
    class SpawnType {
        <<enumeration>>
        Instigated
        Vanguard
        Carmikaze
        Army
        Narration
        ItemHMG
        ItemShotgun
        ItemGranade
        ItemRocket
    }
    class ObjectType {
        <<enumeration>>
        Default
        Player
        Enemy
        Bullet
    }
    class TargetLayer {
        <<enumeration>>
        Background
        Main
        Foreground
    }
    class SpawnPosition {
        <<enumeration>>
        Random
        PlayerPosition
        Min
        Max
    }

    StageManager *-- "*" Wave : owns
    Wave *-- "*" SpawnData
    StageManager ..> EnemyFactory : CreateEnemy
    StageManager ..> ItemFactory : CreateItem
    EnemyFactory ..> EnemyInfo : uses
    ItemFactory ..> ItemInfo : uses
    SpawnData --> SpawnType
    EnemyInfo --> SpawnType
    ItemInfo --> SpawnType

    AudioManager o-- "*" PlayingSoundInfo : manages list
    ImageLoader ..> Sprite : creates / caches
    ObjectPool ..> Bullet : pools
    ObjectPool ..> Explosion : pools
```

> 팩토리가 생성하는 `Enemy`/`WeaponItem`, 풀링 대상 `Bullet`/`Explosion`의 정의는 [game-objects.md](game-objects.md), 스테이지를 구동하는 `GameManager`는 [core-system.md](core-system.md) 참고.
