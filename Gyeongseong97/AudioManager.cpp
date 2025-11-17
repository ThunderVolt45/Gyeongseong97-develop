#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <iostream>

using namespace std;

bool InitializeAudio()
{
    ma_result result;
    ma_engine engine;

    result = ma_engine_init(NULL, &engine);

    if (result != MA_SUCCESS)
    {
        cerr << "miniaudio: 오디오 엔진 초기화 실패!" << endl;
        return false;
    }

    cout << "miniaudio: 오디오 엔진 초기화 성공" << endl;
    return true;
}