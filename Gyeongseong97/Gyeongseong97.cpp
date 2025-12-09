#pragma execution_character_set( "utf-8" )

#include <windows.h>
#include <stdlib.h>

#include "GameApp.h"

int main()
{
	// 유니코드 및 콘솔 출력 설정 (UTF-8)
	SetConsoleOutputCP(CP_UTF8);

	// 프로그램의 진입점 역할만 해준다
	GameApp app;
	app.Run();

	return 0;
}