#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "AudioManager.h"
#include "Utility.h"

#include <iostream>
#include <vector>
#include <thread>
#include <windows.h>

const static std::wstring AUDIO_PATH = L"\\Sounds\\";

AudioManager::AudioManager()
{
	ma_result result;

	// 오디오 엔진 초기화
	// 별도 설정 없이 NULL로 초기화하면 기본 오디오 장치를 사용하게 됨
	result = ma_engine_init(NULL, &audioEngine);

	if (result != MA_SUCCESS)
	{
		std::wcerr << L"miniaudio : 오디오 엔진을 초기화하는데 실패했습니다." << std::endl;
	}
	else
	{
		std::wcout << L"miniaudio : 오디오 엔진 초기화 성공!" << std::endl;
	}
}

AudioManager::~AudioManager()
{
	// 모든 활성 사운드 리소스를 정리
	for (ma_sound* pSound : sounds)
	{
		ma_sound_uninit(pSound);
		delete pSound;
	}

	// 오디오 엔진 리소스 정리
	ma_engine_uninit(&audioEngine);
}

void AudioManager::PlayAudio(std::wstring audioFile, bool loop)
{
	// 파일 경로 정의
	std::vector<wchar_t> pathBuffer(MAX_PATH);
	DWORD ret = GetCurrentDirectoryW(MAX_PATH, pathBuffer.data());

	if (ret == 0)
	{
		std::wcerr << L"Error: 파일 경로를 가져오는데 실패했습니다." << std::endl;
		return;
	}
	else if (ret == pathBuffer.size())
	{
		std::wcerr << L"Warning: 파일 경로가 너무 깁니다.더 큰 길이 버퍼가 필요합니다." << std::endl;
	}

	auto strFilePath = pathBuffer.data() + AUDIO_PATH + audioFile;
	auto filePath = Utility::ConvertWideToUtf8(strFilePath.c_str());

	// ma_sound 객체를 생성
	ma_sound* pSound = new ma_sound;
	ma_result result = ma_sound_init_from_file(&audioEngine, filePath.c_str(), 0, NULL, NULL, pSound);

	if (result != MA_SUCCESS)
	{
		std::wcerr << L"miniaudio : " << strFilePath << L" 파일을 불러오는데 실패했습니다." << std::endl;
		delete pSound;
		return;
	}

	// 사운드의 포인터를 리스트에 추가
	sounds.push_back(pSound);
	
	// 스레드 생성
	std::thread audioThread = std::thread(&AudioManager::PlayAudioThread, this, pSound, loop);
	audioThread.detach();
}

void AudioManager::PlayAudioThread(ma_sound* pSound, bool loop)
{
	// 반복 재생 설정
	ma_sound_set_looping(pSound, loop ? MA_TRUE : MA_FALSE);

	// 사운드 재생
	ma_sound_start(pSound);

	// 반복 재생하지 않는 사운드일 경우 대기 후 리스트에서 제거하고 리소스를 정리한다
	while (!loop)
	{
		if (ma_sound_at_end(pSound) != MA_FALSE)
		{
			ma_sound_uninit(pSound);
			sounds.remove(pSound);
			delete pSound;

			return;
		}
	}
}