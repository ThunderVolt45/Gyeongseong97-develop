#pragma execution_character_set( "utf-8" )

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "AudioManager.h"
#include "Utility.h"
#include "GameConstants.h"

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <filesystem>

#pragma region Constructer & Destroyer

AudioManager::AudioManager()
{
	ma_result result;

	// 오디오 엔진 초기화
	// 별도 설정 없이 NULL로 초기화하면 기본 오디오 장치를 사용하게 됨
	result = ma_engine_init(NULL, &audioEngine);

	if (result != MA_SUCCESS)
	{
		std::cerr << "miniaudio : 오디오 엔진을 초기화하는데 실패했습니다." << std::endl;
	}
	else
	{
		std::cout << "miniaudio : 오디오 엔진 초기화 성공!" << std::endl;
	}
}

AudioManager::~AudioManager()
{
	// 뮤텍스를 사용하여 리스트 접근 보호
	std::lock_guard<std::mutex> lock(soundMutex);

	// 모든 활성 사운드 리소스를 정리
	for (PlayingSoundInfo* info : activeSounds)
	{
		ma_sound_uninit(info->pSound);

		delete info->pSound; // ma_sound 객체 해제
		delete info; // PlayingSoundInfo 객체 해제
	}

	// 리스트 비우기
	activeSounds.clear();

	// 오디오 엔진 리소스 정리
	ma_engine_uninit(&audioEngine);
}

#pragma endregion


AudioManager& AudioManager::GetInstance()
{
	static AudioManager instance;
	return instance;
}

void AudioManager::PlayAudio(std::wstring audioPath, float volume, bool loop)
{
	// 오디오 경로를 구한다
	std::filesystem::path currentPath = std::filesystem::current_path();

	auto strFilePath = Utility::ConvertWideToUtf8(currentPath.c_str()) + Utility::ConvertWideToUtf8((AUDIO_PATH.data() + audioPath).c_str());
	auto filePath = strFilePath.c_str();

	// ma_sound 객체를 생성
	ma_sound* pSound = new ma_sound;

	ma_result result = ma_sound_init_from_file(&audioEngine, filePath, 0, NULL, NULL, pSound);

	if (result != MA_SUCCESS)
	{
		std::cerr << "miniaudio : " << filePath << " 파일을 불러오는데 실패했습니다." << std::endl;
		delete pSound;
		return;
	}

	// 사운드 볼륨 설정
	ma_sound_set_volume(pSound, volume);

	// PlayingSoundInfo 객체 생성 및 초기화
	PlayingSoundInfo* pInfo = new PlayingSoundInfo(pSound, audioPath, loop);

	{
		// 뮤텍스를 사용하여 리스트 접근 보호
		std::lock_guard<std::mutex> lock(soundMutex);

		// 포인터를 리스트에 추가한다.
		activeSounds.push_back(pInfo);
	}
	
	// 소리 재생을 메인 스레드에서 실행하면 재생이 끝날 때까지
	// 메인 스레드가 묶이므로 소리를 재생하기 위한 백그라운드 스레드를 하나 생성한다.
	std::thread audioThread = std::thread(&AudioManager::PlayAudioThread, this, pInfo);
	audioThread.detach();
}

void AudioManager::SetMasterVolume(float volume)
{
	// miniaudio 엔진의 마스터 볼륨 설정
	ma_engine_set_volume(&audioEngine, volume);
}

void AudioManager::StopAudio(std::wstring audioPath)
{
	// 뮤텍스를 사용하여 리스트 접근 보호
    std::lock_guard<std::mutex> lock(soundMutex);

    // activeSounds 리스트를 순회하며 해당 audioPath를 가진 사운드를 찾음
    for (PlayingSoundInfo* pInfo : activeSounds)
    {
		// 해당 오디오 경로를 가진 모든 재생 중인 사운드를 중지시킨다.
        if (pInfo->audioPath == audioPath)
        {
			pInfo->Stop();
        }
    }
}

void AudioManager::FadeOutAudio(std::wstring audioPath, float fadeOutTimeInMS)
{
	// 뮤텍스를 사용하여 리스트 접근 보호
	std::lock_guard<std::mutex> lock(soundMutex);

	// activeSounds 리스트를 순회하며 해당 audioPath를 가진 사운드를 찾음
	for (PlayingSoundInfo* pInfo : activeSounds)
	{
		// 해당 오디오 경로를 가진 모든 재생 중인 사운드를 페이드 아웃시킨다.
		if (pInfo->audioPath == audioPath)
		{
			pInfo->FadeOut(fadeOutTimeInMS);
		}
	}
}

bool AudioManager::IsPlaying(std::wstring audioPath)
{
	// activeSounds 리스트를 순회하며 해당 audioPath를 가진 사운드를 찾음
	for (PlayingSoundInfo* pInfo : activeSounds)
	{
		// 해당 오디오 경로를 가진 모든 재생 중인 사운드를 페이드 아웃시킨다.
		if (pInfo->audioPath == audioPath)
		{
			return pInfo->IsPlaying();
		}
	}

	return false;
}

void AudioManager::PlayAudioThread(PlayingSoundInfo* pInfo)
{
	pInfo->Play();

	// 사운드가 재생 중인 동안 대기
	while (pInfo->IsPlaying())
	{
		pInfo->Update();

		// CPU 과점유를 막기 위해 잠깐 대기
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	{
		// 뮤텍스를 사용하여 리스트 접근 보호
		std::lock_guard<std::mutex> lock(soundMutex);

		// 리스트에서 해당 PlayingSoundInfo 제거
		activeSounds.remove(pInfo);
	}

	// 메모리 해제
	delete pInfo;
}
