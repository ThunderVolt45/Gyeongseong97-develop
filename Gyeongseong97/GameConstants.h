#pragma once

#include <chrono>
#include <string>

using namespace std;

// 게임 설정 상수
constexpr int GAME_WIDTH = 240; // 기본값 : 160,
constexpr int GAME_HEIGHT = 180; // 기본값 : 120, 캔버스 높이는 텍스트 높이의 2배 (Block 기준)
constexpr int SCORE_FOR_HIT = 100; // 총알 히트 당 점수
constexpr std::chrono::microseconds TICK_TIME = 16700us; // 16667us = 약 60fps

// 플레이어 기본 위치
constexpr int PLAYER_DEFAULT_POSITION_X = GAME_WIDTH / 2;
constexpr int PLAYER_DEFAULT_POSITION_Y = GAME_HEIGHT - 20;

// 공간 분할을 위한 그리드 설정
constexpr int CELL_SIZE = 20; // 격자 크기 (20x20)
constexpr int GRID_COLS = (GAME_WIDTH + CELL_SIZE - 1) / CELL_SIZE; // 160 / 20 = 8
constexpr int GRID_ROWS = (GAME_HEIGHT + CELL_SIZE - 1) / CELL_SIZE; // 120 / 20 = 6

// 사운드 이름 상수
constexpr float BGM_VOULME = 0.5f;
constexpr wstring_view BGM_MAIN{ L"bgm_hk97_16bit.mp3" };
constexpr wstring_view BGM_BOSS{ L"bgm_hero.mp3" };
constexpr wstring_view SFX_GUNFIRE{ L"sfx_gunfire.wav" };
constexpr wstring_view SFX_CAR{ L"sfx_car.wav" };
constexpr wstring_view SFX_EXPLOSION{ L"sfx_explosion.wav" };