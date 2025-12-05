#pragma once

#include <map>
#include <windows.h>

// 키 코드
constexpr int KEYCODE_LEFT = VK_LEFT;
constexpr int KEYCODE_RIGHT = VK_RIGHT;
constexpr int KEYCODE_UP = VK_UP;
constexpr int KEYCODE_DOWN = VK_DOWN;
constexpr int KEYCODE_SPACE = VK_SPACE;
constexpr int KEYCODE_Z = 0x005A; // Z

class InputManager
{
private:
    /// <summary>
    /// InputManager의 생성자
    /// private으로 선언하여 외부에서 직접 개체를 생성하는 것을 방지한다.
    /// </summary>
    InputManager();

    // 복사 생성자와 대입 연산자를 삭제해 인스턴스가 복제되지 못하게 한다.
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    // 현재 키 입력 상태를 저장한다
    // An array is efficient for Windows virtual key codes (0-255)
    bool keyStates[224]; // Standard virtual key codes go up to 0xE0 (224)

public:
    /// <summary>
    /// InputManager의 유일한 인스턴스에 접근하는 메서드
    /// </summary>
    /// <returns>GameManager 객체의 참조값</returns>
    static InputManager& GetInstance();

    // 추적중인 모든 키의 상태를 갱신한다
    void Update();

    /// <summary>
    /// 현재 키가 눌려 있는지 여부를 검사한다
    /// </summary>
    /// <param name="key_code">VK_KEYCODE</param>
    /// <returns>키가 지금 눌려있는지 여부</returns>
    bool IsKeyDown(int key_code) const;
    
    /// <summary>
    /// 키가 (지속적으로) 눌리는 중인지 여부를 검사한다
    /// </summary>
    /// <param name="key_code">VK_KEYCODE</param>
    /// <returns>키가 지속적으로 눌리는 중인지 여부</returns>
    bool IsKeyPressed(int key_code) const;

private:
    // IsKeyPressed 처리를 위해 이전 키 상태를 저장한다
    bool previousKeyStates[224];
};
