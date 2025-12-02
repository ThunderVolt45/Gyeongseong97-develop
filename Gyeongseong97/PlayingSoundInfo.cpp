
#include "miniaudio.h"
#include "PlayingSoundInfo.h"

#pragma region PlayingSoundInfo

PlayingSoundInfo::PlayingSoundInfo(ma_sound* pSound, std::wstring audioPath, bool isLooping)
{
	this->pSound = pSound;
	this->audioPath = audioPath;
	this->isLooping = isLooping;
}

PlayingSoundInfo::~PlayingSoundInfo()
{
	// 리소스 정리
	ma_sound_uninit(pSound);
	delete pSound;
}

void PlayingSoundInfo::Play()
{
	// 반복 재생 설정
	ma_sound_set_looping(pSound, isLooping ? MA_TRUE : MA_FALSE);

	// 사운드 재생
	ma_sound_start(pSound);
}

void PlayingSoundInfo::Stop()
{
	// 재생 중지
	ma_sound_stop(pSound);
}

void PlayingSoundInfo::FadeOut(float milliseconds)
{
	isFading = true;
	// 현재 볼륨에서 시작해서 0까지 페이드 아웃
	ma_sound_set_fade_in_milliseconds(pSound, -1, 0, milliseconds);
}

bool PlayingSoundInfo::IsPlaying()
{
	return ma_sound_is_playing(pSound);
}

void PlayingSoundInfo::Update()
{
	if (isFading && ma_sound_get_volume(pSound) <= 0.01f)
	{
		Stop();
	}
}

#pragma endregion