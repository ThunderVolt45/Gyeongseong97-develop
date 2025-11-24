#pragma once
#include "miniaudio.h"

#include <iostream>
#include <string>
#include <list>

class AudioManager
{
private:
	ma_engine audioEngine;
	std::list<ma_sound*> sounds;
	void PlayAudioThread(ma_sound* pSound, bool loop = false);

public:
	AudioManager();
	~AudioManager();
	void PlayAudio(std::wstring audioPath, bool loop = false);
};

