# 핵심 시스템 (Core System)

게임 애플리케이션의 진입점과 메인 루프, 입력·렌더·충돌·유틸리티를 담당하는 영역입니다.

- **`GameApp`**: 애플리케이션 진입점. `AppState`에 따라 타이틀/컷씬/도움말/크레딧/게임 루프 화면을 전환.
- **`GameManager` (Singleton)**: 게임 핵심 루프. 모든 `GameObject`를 `list`로 관리하고 `Player`·`StageManager`를 직접 소유.
- **`InputManager` (Singleton)**: WinAPI 기반 키 입력 상태 추적.
- **`RenderSystem`**: FTXUI 기반 화면 렌더링 (정적 유틸).
- **`CollisionManager`**: 게임 오브젝트 간 충돌 처리 (정적 유틸).
- **`Utility`**: 문자열 변환·난수·파일 읽기 등 보조 함수 (정적 유틸).

```mermaid
classDiagram
    direction LR

    class GameApp {
        - AppState currentAppState
        + Run() void
        - ClearScreen() void
        - TitleScreen(screen) AppState
        - DrawCutscene(screen, imageName, line1..4, nextState) AppState
        - HowToPlay(screen) AppState
        - Credits(screen) AppState
        - GameLoop(screen) AppState
    }

    class GameManager {
        <<Singleton>>
        - StageManager stageManager
        - set~GameObject*~ objectsToDestroy
        - vector~GameObject~ objectsToCreate
        + Player player
        + long long tick
        + recursive_mutex gameMutex
        + list~GameObject~ gameObjects
        + atomic~int~ score
        + atomic~bool~ IsRunning
        + atomic~bool~ IsGamePause
        + atomic~bool~ IsGameOver
        + atomic~bool~ IsGameClear
        + atomic~int~ currentFps
        + atomic~int~ currentLps
        + wstring gameTime
        + function onResetCallback
        - UpdateGameObjects(objects) void
        - EraseGameObjects() void
        + GetInstance() GameManager$
        + Initialize() void
        + Reset() void
        + Update() void
        + OnEvent(screen, event) bool
        + CreateGameObject(obj, layer) void
        + DestroyGameObject(obj) void
        + DestroyAllEnemiesExcept(except) void
        + IsEnemyAlive() bool
        + GetEnemyCount() int
    }

    class InputManager {
        <<Singleton>>
        - bool keyStates[224]
        - bool previousKeyStates[224]
        + GetInstance() InputManager$
        + Update() void
        + IsKeyDown(key_code) bool
        + IsKeyPressed(key_code) bool
    }

    class RenderSystem {
        - string errorMessage$
        - bool isErrorActive$
        - long long errorDisplayStartTime$
        - DrawObjectSprite(canvas, object) void$
        + RenderGameScreen() Element$
        + RenderCutscene(imageName, line1..4) Element$
        + RenderCredits(lines, startPadding, scrollY) Element$
        + ShowErrorMessage(message) void$
        + ClearErrorMessage() void$
    }

    class CollisionManager {
        + ProcessCollisions(gameObjects, player, objectsToDestroy) void$
    }

    class Utility {
        + ConvertWideToUtf8(wideStr) string$
        + GenerateRandomNumber(begin, end) int$
        + ReadTextFile(fileName, targetVector) void$
    }

    class AppState {
        <<enumeration>>
        TITLE_SCREEN
        INTRO_CUTSCENE
        HOW_TO_PLAY
        GAME_PLAYING
        CREDITS
        EXIT
    }

    GameApp ..> GameManager : runs GameLoop
    GameApp ..> RenderSystem : RenderCutscene / RenderCredits
    GameApp --> AppState : state machine
    GameManager ..> CollisionManager : ProcessCollisions
    GameManager ..> RenderSystem : RenderGameScreen
```

> 게임 오브젝트 계층과의 관계(`GameManager`가 `Player`/`GameObject`를 소유)는 [game-objects.md](game-objects.md) 참고.
