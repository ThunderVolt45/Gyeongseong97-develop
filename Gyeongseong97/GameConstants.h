#pragma once
#include <chrono>
#include <string>

using namespace std;

// 게임 기본 설정 상수
constexpr int GAME_WIDTH = 240; // 기본값 : 160,
constexpr int GAME_HEIGHT = 180; // 기본값 : 120, 캔버스 높이는 텍스트 높이의 2배 (Block 기준)
constexpr int SCORE_FOR_HIT = 100; // 총알 히트 당 점수
constexpr std::chrono::microseconds TICK_TIME = 16700us; // 16667us = 약 60fps
constexpr int ERROR_DISPLAY_DURATION_TICKS = 300; // 5 * 초당 60틱

// 게임 난이도 관련 설정 상수
constexpr float COLLISION_DAMAGE = 0.1f; // 상호 간 충돌 데미지

constexpr int DEFAULT_WEAPON_DAMAGE = 1; // 기본 무기 데미지
constexpr int DEFAULT_WEAPON_COOLTIME = 8; // 연사력 (틱)

constexpr int HMG_DAMAGE = 1; // HMG 데미지
constexpr int HMG_COOLTIME = 4; // 연사력 (틱)

constexpr int SHOTGUN_DAMAGE_FOR_PELLET = 1; // 산탄총 팰릿당 데미지
constexpr int SHOTGUN_COOLTIME = 30; // 연사력 (틱)

constexpr int GRENADE_DAMAGE = 5; // 수류탄 데미지
constexpr int GRENADE_COOLTIME = 30; // 연사력 (틱)

constexpr int ROCKET_DAMAGE = 3; // 로켓 데미지
constexpr int ROCKET_COOLTIME = 30; // 연사력 (틱)
constexpr int ROCKET_MAX_LIFETICK = 120; // 로켓 총알의 최대 수명

// 플레이어 기본 위치
constexpr int PLAYER_DEFAULT_POSITION_X = GAME_WIDTH / 2;
constexpr int PLAYER_DEFAULT_POSITION_Y = GAME_HEIGHT - 20;

// 충돌 계산시 공간 분할을 위한 그리드 설정
constexpr int CELL_SIZE = 20; // 격자 크기 (20x20)
constexpr int GRID_COLS = (GAME_WIDTH + CELL_SIZE - 1) / CELL_SIZE;
constexpr int GRID_ROWS = (GAME_HEIGHT + CELL_SIZE - 1) / CELL_SIZE;

// 폭☆8 상수
constexpr int EXPLOSION_DEFAULT_SIZE_X = 40;
constexpr int EXPLOSION_DEFAULT_SIZE_Y = 30;

// 리소스 경로
constexpr wstring_view IMAGE_PATH{ L"\\Sprites\\" };
constexpr wstring_view AUDIO_PATH{ L"\\Sounds\\" };

// 사운드 리소스
constexpr float BGM_VOULME = 0.5f;
constexpr wstring_view BGM_MAIN{ L"bgm_hk97_16bit.mp3" };
constexpr wstring_view BGM_BOSS{ L"bgm_hero.mp3" };
constexpr wstring_view BGM_WIN{ L"bgm_outro.mp3" };
constexpr wstring_view SFX_GUNFIRE{ L"sfx_gunfire.wav" };
constexpr wstring_view SFX_CAR{ L"sfx_car.wav" };
constexpr wstring_view SFX_EXPLOSION{ L"sfx_explosion.wav" };
constexpr wstring_view SFX_GETWEAPON{ L"sfx_get.wav" };
constexpr wstring_view SFX_GRENADE{ L"sfx_grenade.wav" };
constexpr wstring_view SFX_ROCKET{ L"sfx_rocket.wav" };

// 이미지 리소스
constexpr wstring_view SPR_PLAYER{ L"player.png" };
constexpr wstring_view SPR_PLAYER_DEATH{ L"player_death.png" };
constexpr wstring_view SPR_INSTIGATED{ L"enemy_Instigated.png" };
constexpr wstring_view SPR_VANGUARD{ L"enemy_vanguard.png" };
constexpr wstring_view SPR_ARMY{ L"enemy_army.png" };
constexpr wstring_view SPR_CARMIKAZE{ L"enemy_Carmikaze.png" };
constexpr wstring_view SPR_NARRATION{ L"enemy_narration.png" };
constexpr wstring_view SPR_HMG{ L"item_hmg.png" };
constexpr wstring_view SPR_BOMB{ L"item_bomb.png" };
constexpr wstring_view SPR_SHOTGUN{ L"item_shotgun.png" };
constexpr wstring_view SPR_ROCKET{ L"item_rocket.png" };

// 기타 리소스
constexpr wstring_view PATH_CREDIT{ L"CREDIT.md" };
constexpr wstring_view PATH_LICENSE{ L"LICENSE" };
constexpr wstring_view PATH_THIRD_PARTY_NOTICE{ L"THIRD_PARTY_NOTICES.md" };

// 기타 문자열 상수
constexpr wstring_view SPR_EXPLOSION_PREFIX{ L"폭8\\폭8_" };
constexpr wstring_view POSTFIX_PNG{ L".png" };