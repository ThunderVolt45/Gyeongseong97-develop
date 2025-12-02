#pragma once
#include "miniaudio.h"

#include <iostream>
#include <string>

class PlayingSoundInfo
{
public:
	ma_sound* pSound;
	std::wstring audioPath; // 재생 중인 사운드의 이름
	bool isLooping; // 반복 재생 여부
	bool isFading = false; // 페이드 아웃 중인지

	PlayingSoundInfo(ma_sound* pSound, std::wstring audioPath, bool isLooping);
	~PlayingSoundInfo();

	void Play();
	void Stop();
	void FadeOut(float milliseconds);
	bool IsPlaying();
	void Update();
};
