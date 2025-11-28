#pragma once
#include "miniaudio.h"

#include <iostream>
#include <string>
#include <list>
#include <mutex>

class AudioManager
{
	ma_engine audioEngine;
	
	struct PlayingSoundInfo {
		ma_sound* pSound;
		std::wstring audioPath; // For identifying which sound to stop
		bool isLooping;       // To track if it's a looping sound
	};
	std::list<PlayingSoundInfo*> activeSounds; // List of currently playing sounds

	std::mutex soundMutex; // Mutex for activeSounds list

	/// <summary>
	/// AudioManager의 생성자
	/// private으로 선언하여 외부에서 직접 개체를 생성하는 것을 방지한다.
	/// </summary>
	AudioManager();

	/// <summary>
	/// AudioManager의 소멸자.
	/// private으로 선언하여 C++ 런타임이 직접 소멸시키도록 한다.
	/// </summary>
	~AudioManager();

	/// 복사 생성자와 대입 연산자를 삭제해 인스턴스가 복제되지 못하게 한다.
	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;

	/// <summary>
	/// AudioManager 내부적으로 백그라운드 오디오 재생을 위해 Thread를 생성하는 함수
	/// </summary>
	/// <param name="pSoundInfo">재생할 사운드의 정보</param>
	void PlayAudioThread(PlayingSoundInfo* pSoundInfo);

public:
	/// <summary>
	/// AudioManager의 유일한 인스턴스에 접근하는 메서드
	/// </summary>
	/// <returns>AudioManager 객체의 참조값</returns>
	static AudioManager& GetInstance();

	/// <summary>
	/// 오디오 파일을 재생합니다.
	/// </summary>
	/// <param name="audioPath">오디오 파일의 이름</param>
	/// <param name="volume">재생 볼륨 (0.0f ~ 1.0f)</param>
	/// <param name="loop">반복 재생 여부</param>
	void PlayAudio(std::wstring audioPath, float volume = 1.0f, bool loop = false);

	/// <summary>
	/// 모든 오디오의 마스터 볼륨을 조절합니다.
	/// </summary>
	/// <param name="volume">마스터 볼륨 (0.0f ~ 1.0f)</param>
	void SetMasterVolume(float volume);

	/// <summary>
	/// 현재 재생 중인 오디오 파일의 재생을 중단합니다.
	/// 반복 재생 중인 오디오만 중단이 가능합니다.
	/// </summary>
	/// <param name="audioPath">오디오 파일의 이름</param>
	void StopAudio(std::wstring audioPath);
};

