#include "InputManager.h"
#include "RenderSystem.h"

InputManager::InputManager()
{
    // 모든 키의 상태를 false로 설정한다
    for (int i = 0; i < 224; ++i)
    {
        keyStates[i] = false;
        previousKeyStates[i] = false;
    }
}

InputManager& InputManager::GetInstance()
{
    static InputManager instance;
    return instance;
}

void InputManager::Update()
{
    // 현재 키 상태를 이전 키 상태로 복제
    for (int i = 0; i < 224; ++i)
    {
        previousKeyStates[i] = keyStates[i];
    }

    // 현재 키 상태를 GetAsyncKeyState로 갱신한다
    // Only check for keys we might care about to optimize (e.g., A-Z, 0-9, VK_LBUTTON, VK_RBUTTON, arrow keys, space, etc.)
    // For simplicity, checking a common range of virtual key codes.
    // Full list of virtual key codes: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    for (int i = 1; i < 224; ++i) // VK_LBUTTON이 0x01 이기 때문에 1부터 시작
    {
        keyStates[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }
}

bool InputManager::IsKeyDown(int key_code) const
{
    if (key_code >= 0 && key_code < 224)
    {
        return keyStates[key_code];
    }

    return false;
}

bool InputManager::IsKeyPressed(int key_code) const
{
    if (key_code >= 0 && key_code < 224)
    {
        return keyStates[key_code] && !previousKeyStates[key_code];
    }

    return false;
}