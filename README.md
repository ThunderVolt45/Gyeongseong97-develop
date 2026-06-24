# 京城 97 (Gyeongseong 97)

> **WinAPI + FTXUI + miniaudio 로 만든 C++ 콘솔(터미널) 종스크롤 슈팅 게임 포트폴리오**

![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?logo=windows&logoColor=white)
![IDE](https://img.shields.io/badge/Visual%20Studio-2022-5C2D91?logo=visualstudio&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green.svg)

> [!NOTE]
> 이 게임은 제작자의 정치적 성향과는 어떠한 연관도 없으며, 그냥 포트폴리오 겸 사람들 웃기려고 만든 게임이라는 것을 알립니다.

---

## 🎮 게임 소개

**京城97 (이하 경성97)** 은 불량 게임 **홍콩97** 과 역사왜곡(...) 드라마 **야인시대** 를 패러디 및 풍자한 종스크롤 슈팅 게임입니다.

플레이어는 백의사의 지령을 받은 백색 테러주의자 **김두한** 을 조종해 공산당 연극 단체 수장 '심영'의 선전선동에 세뇌된 학생과 시민들, 공산당 전위대들, 그리고 최종병기 **'나레이션 양반'** 을 처치해 1972억 1121만 빨갱이로 가득 차버린 조선 천지에 평화를 가져와야 합니다.

화면에 나타나는 적들을 처치하며 최대한 많은 점수를 쌓는 것이 목표이며, 보스 몬스터 '나레이션 양반' 을 처치하면 게임에서 승리합니다.

> 이 모든 것이 **터미널(콘솔) 창** 안에서 컬러 픽셀 렌더링으로 돌아갑니다.

---

## ✨ 주요 특징

- 🖥️ **터미널 위의 실시간 그래픽** — FTXUI 캔버스로 스프라이트를 컬러 렌더링하는 TUI 게임
- 🎯 **5종 무기 시스템** — 기본 / 중기관총 / 산탄총 / 수류탄 / 로켓 (Strategy 패턴)
- 👹 **9단계 패턴을 가진 보스** — 등장·사격·돌진·폭탄·소환·사망까지 (State 패턴)
- 🧟 **다양한 적** — 선동된 시민, 전위대, 자폭 자동차(Carmikaze), 인민군
- ♻️ **오브젝트 풀링** — 총알·폭발 등 빈번한 생성/파괴 객체를 재활용해 성능 확보
- 🔊 **비동기 사운드** — miniaudio 기반 BGM/효과음 재생 및 페이드 처리
- 📜 **스테이지 데이터 외부화** — `stage_data.json` 으로 웨이브 구성 분리
- 🧵 **멀티스레드 게임 루프** — 로직(LPS)과 렌더(FPS)를 분리

---

## 🕹️ 플레이 방법

### 준비물
- **Windows Terminal** 권장
- 그 외 터미널 에뮬레이터에서는 정상적으로 작동하지 않을 수 있습니다.

### 조작키
| 키 | 동작 |
| --- | --- |
| 방향키 (↑ ↓ ← →) | 이동 |
| `Z` | 사격 |
| `X` | 수류탄 |

---

## 🛠️ 기술 스택

| 구분 | 내용 |
| --- | --- |
| 언어 | C++20 |
| 빌드 | Visual Studio 2022 (MSVC v143) |
| 플랫폼 | Windows 10/11 |
| 입력 | **WinAPI** — 실시간 키보드 입력 처리 |
| 렌더링 | **FTXUI** — 실시간 TUI(Terminal User Interface) 렌더링 |
| 이미지 | **stb_image** — PNG 스프라이트 로드 |
| 사운드 | **miniaudio** — BGM/효과음 출력 |
| 데이터 | **nlohmann/json** — 스테이지 데이터 파싱 |

---

## 🚀 빌드 및 실행

> Visual Studio 2022 와 **C++20** 을 지원하는 MSVC 툴셋이 필요합니다.

```bash
# 1. 저장소 클론
git clone https://github.com/ThunderVolt45/Gyeongseong97.git

# 2. 솔루션 열기
#    Gyeongseong97.sln 을 Visual Studio 2022 로 엽니다.

# 3. 빌드 & 실행
#    구성: Release / x64 권장
#    Ctrl + F5 (디버깅 없이 시작)
```

실행 후에는 **Windows Terminal** 에서 플레이하는 것을 권장합니다.

---

## 📁 프로젝트 구조

```
Gyeongseong97/
├── Gyeongseong97.cpp        # 진입점
├── GameApp.*                # 애플리케이션 / 화면 상태 머신
├── GameManager.*            # 메인 게임 루프 (Singleton)
├── StageManager.*           # 웨이브 기반 스테이지 진행
├── GameObject.*             # 모든 게임 개체의 기반 클래스
│   ├── Player.*             # 김두한
│   ├── Enemy.* / Army.* / Vanguard.* / Carmikaze.*
│   ├── Narration.*          # 보스 '나레이션 양반'
│   ├── Bullet.* / Explosion.* / WeaponItem.*
├── BossState*.*             # 보스 행동 (State 패턴)
├── Weapon*.*                # 무기 (Strategy 패턴)
├── EnemyFactory.* / ItemFactory.*   # 객체 생성 (Factory)
├── ObjectPool.h             # 오브젝트 풀링 (Singleton)
├── InputManager.* / AudioManager.*  # 입력 / 사운드 (Singleton)
├── RenderSystem.* / CollisionManager.* / ImageLoader.* / Utility.*
├── stage_data.json          # 스테이지/웨이브 데이터
├── Sprites/                 # 스프라이트 이미지
└── Sounds/                  # BGM / 효과음

docs/
└── class-diagram/           # 클래스 다이어그램 (Mermaid)
```

---

## 🏛️ 아키텍처

객체지향 설계 패턴을 적극 활용했습니다.

- **Singleton** — `GameManager`, `InputManager`, `AudioManager`, `ObjectPool<T>`
- **State** — 보스 `Narration` 의 9개 행동 상태 전환
- **Strategy** — `Weapon` 의 무기별 발사 로직 교체
- **Factory** — `EnemyFactory` / `ItemFactory` 의 객체 생성
- **Object Pool** — `Bullet` / `Explosion` 재활용

> 📐 상세 클래스 다이어그램은 **[docs/class-diagram](docs/class-diagram/README.md)** 에서 영역별로 확인할 수 있습니다.

---

## 🙏 크레딧

기획·개발: **[ThunderVolt45](https://github.com/ThunderVolt45)**

원작·그래픽·사운드 등 전체 출처는 **[CREDIT.md](CREDIT.md)** 를 참고하세요.

---

## 📄 라이선스

이 프로젝트는 **[MIT License](LICENSE)** 를 따릅니다.

사용된 오픈소스 라이브러리(FTXUI, nlohmann/json, stb_image, miniaudio)의 라이선스 고지는 **[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)** 를 참고하세요.

> 게임에 사용된 일부 그래픽·사운드 리소스는 원저작자에게 권리가 있으며, 본 프로젝트는 비상업적 패러디 목적으로 제작되었습니다.
