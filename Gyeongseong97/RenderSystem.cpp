#include "RenderSystem.h"
#include "GameManager.h"
#include "GameConstants.h"
#include "Enums.h"

std::string RenderSystem::errorMessage = "";
bool RenderSystem::isErrorActive = false;
long long RenderSystem::errorDisplayStartTime = 0;

using namespace ftxui;

void RenderSystem::DrawObjectSprite(ftxui::Canvas& canvas, const GameObject& object)
{
	int spriteIndex = 0;

	// 오브젝트 좌표 값을 int로 반올림해 형변환
	int objX = static_cast<int>(std::round(object.x));
	int objY = static_cast<int>(std::round(object.y));

	// 만약 Sprite 정보가 없다면 흰색 블럭 하나를 찍는다
	if (object.sprite.sizeX == 0)
	{
		canvas.DrawBlock(objX, objY, true, ftxui::Color::White);
		canvas.DrawBlock(objX + 1, objY, true, ftxui::Color::White);
		canvas.DrawBlock(objX, objY + 1, true, ftxui::Color::White);
		canvas.DrawBlock(objX + 1, objY + 1, true, ftxui::Color::White);

		return;
	}

	// Sprite의 색상 값을 보고 Canvas에 블럭을 그린다.
	for (int y = objY; y < objY + object.sprite.sizeY; y++)
	{
		// Sprite의 위가 잘리는 경우
		if (y < 0)
		{
			spriteIndex += object.sprite.sizeX;
			continue;
		}

		// Sprite의 아래가 잘리는 경우
		if (y >= GAME_HEIGHT)
		{
			break;
		}

		for (int x = objX; x < objX + object.sprite.sizeX; x++)
		{
			// Sprite의 왼쪽이 잘리는 경우
			if (x < 0)
			{
				spriteIndex++;
				continue;
			}

			// Sprite의 오른쪽이 잘리는 경우
			if (x >= GAME_WIDTH)
			{
				spriteIndex++;
				continue;
			}

			// index 값이 vector 크기를 넘을 경우 중단
			if (spriteIndex >= object.sprite.colors.size())
			{
				break;
			}

			// Canvas에 색상으로 점(블록)을 찍는다.
			ftxui::Color pixelColor = object.sprite.colors[spriteIndex];
			if (pixelColor.IsOpaque())
			{
				canvas.DrawBlock(x, y, true, pixelColor);
			}
			spriteIndex++;
		}
	}
}

void RenderSystem::ShowErrorMessage(const std::string& message) {

    errorMessage = message;

    isErrorActive = true;

    errorDisplayStartTime = GameManager::GetInstance().tick;

}

void RenderSystem::ClearErrorMessage()
{
    errorMessage = "";
    isErrorActive = false;
}

ftxui::Element RenderSystem::RenderGameScreen()
{
	GameManager& gameManager = GameManager::GetInstance();

	if (isErrorActive && (gameManager.tick - errorDisplayStartTime > ERROR_DISPLAY_DURATION_TICKS)) {
		ClearErrorMessage();
	}

	// 캔버스 생성
	auto canvas = ftxui::Canvas(GAME_WIDTH, GAME_HEIGHT);

	// 임계 구역 설정
	{
		// Update와 Render가 동시에 호출되어 벌어지는 참사를 막기 위해 mutex로 잠가버린다.
		// std::lock_guard를 쓰면 lock_guard가 소멸될 때 자동으로 mutex가 해제된다.
		std::lock_guard<std::recursive_mutex> lock(gameManager.gameMutex);

		auto background = std::vector<std::shared_ptr<GameObject>>();
		auto main = std::vector<std::shared_ptr<GameObject>>();
		auto foreground = std::vector<std::shared_ptr<GameObject>>();

		// 레이어 별 분류
		for (const auto& object : gameManager.gameObjects)
		{
			switch (object->layer)
			{
			case TargetLayer::Background:
				background.push_back(object);
				break;
			case TargetLayer::Foreground:
				foreground.push_back(object);
				break;
			default:
				main.push_back(object);
				break;
			}
		}

		// Background 레이어 그리기
		for (const auto& object : background)
		{
			DrawObjectSprite(canvas, *object);
		}

		// Main 레이어 그리기
		for (const auto& object : main)
		{
			DrawObjectSprite(canvas, *object);
		}

		// 플레이어 그리기
		DrawObjectSprite(canvas, gameManager.player);

		// Foreground 레이어 그리기
		for (const auto& object : foreground)
		{
			DrawObjectSprite(canvas, *object);
		}
	}

	// FPS 표시
	canvas.DrawText(0, 0, 
		"FPS : " + std::to_string(gameManager.currentFps) 
		+ ", Logic : " + std::to_string(gameManager.currentLps));

	// 일시정지시 일시정지 메시지 표시
	if (gameManager.IsGamePause)
	{
		canvas.DrawText(GAME_WIDTH / 2 - (5 * 2) - 2, GAME_HEIGHT / 2 - 8,
			"GAME PAUSE",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::Black;
				p.background_color = ftxui::Color::Yellow1;
			});
		canvas.DrawText(GAME_WIDTH / 2 - (15 * 2) - 2, GAME_HEIGHT / 2,
			"게임이 일시정지 되었습니다.",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::White;
			});
		canvas.DrawText(GAME_WIDTH / 2 - (19 * 2) - 2, GAME_HEIGHT / 2 + 8,
			"(Esc키를 눌러 계속, R키를 눌러 재시작, Q키를 눌러 나가기)",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::White;
			});
	}

	// 게임 오버 시 게임 오버 메시지 표시
	if (gameManager.IsGameOver)
	{
		canvas.DrawText(GAME_WIDTH / 2 - (5 * 2) - 2, GAME_HEIGHT / 2 - 8, 
			"GAME OVER!",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::Black;
				p.background_color = ftxui::Color::Red;
			});
		canvas.DrawText(GAME_WIDTH / 2 - (12 * 2) - 2, GAME_HEIGHT / 2, 
			"김두한은 오렌지병이었던 고혈압으로 쓰러졌다.",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::White;
			});
		canvas.DrawText(GAME_WIDTH / 2 - (12 * 2) - 2, GAME_HEIGHT / 2 + 8, 
			"(R키를 눌러 재시작, Q키를 눌러 나가기)",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::White;
			});
	}
	else if (gameManager.IsGameClear)
	{
		canvas.DrawText(GAME_WIDTH / 2 - (4 * 2) - 2, GAME_HEIGHT / 2 - 8, 
			"YOU WIN!",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::Black;
				p.background_color = ftxui::Color::Green;
			});
		canvas.DrawText(GAME_WIDTH / 2 - (28 * 2) - 2, GAME_HEIGHT / 2, 
			"김두한은 사악한 공산당원들을 모조리 다 쓸어버렸다!",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::White;
			});
		canvas.DrawText(GAME_WIDTH / 2 - (12 * 2) - 2, GAME_HEIGHT / 2 + 8, 
			"(R키를 눌러 재시작, Q키를 눌러 나가기)",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::White;
			});
	}

	// (존재한다면) 에러 메시지 표시
	if (isErrorActive)
	{
		canvas.DrawText(0, GAME_HEIGHT - 1, errorMessage, [](ftxui::Pixel& p) {
			p.foreground_color = ftxui::Color::Red;
			});
	}

	// 캔버스를 박스로 감싸준다
	auto UI = ftxui::canvas(std::move(canvas)) | ftxui::border | ftxui::center;

	// 무기 UI 그리기
	auto currentWeapon = gameManager.player.currentWeapon.get();
	std::wstring remainBullet = currentWeapon->isInfinite ? 
		L"Infinite" : std::to_wstring(currentWeapon->remainBullet) 
						+ L" / " + std::to_wstring(currentWeapon->maxBullet);

	auto weaponUI = ftxui::vbox({
			ftxui::text(L"Bullet : "),
			ftxui::text(remainBullet) | color(ftxui::Color::Yellow)
		});

	// 수류탄 UI 그리기
	auto grenadeWeapon = gameManager.player.grenadeWeapon.get();
	std::wstring remainGrenade = grenadeWeapon->isInfinite ?
		L"Infinite" : std::to_wstring(grenadeWeapon->remainBullet);

	auto grenadeUI = ftxui::vbox({
			ftxui::text(L"Grenade : "),
			ftxui::text(remainGrenade) | color(ftxui::Color::Yellow)
		});

	// 체력 바 그리기
	auto healthUI = ftxui::vbox({
			ftxui::text(L"Health : "),
			ftxui::gauge(gameManager.player.health / gameManager.player.maxHealth)
				| color(ftxui::Color::Red) | bgcolor(ftxui::Color::GrayDark)
		});

	// 기타 UI
	std::wstring textTime = L"Time : " + gameManager.gameTime;
	std::wstring textScore = L"Score : " + std::to_wstring(gameManager.score);

	// 게임 화면 생성
	auto final = ftxui::hbox(
		{
			UI | ftxui::size(ftxui::WIDTH, ftxui::Constraint::EQUAL, GAME_WIDTH / 2),
			ftxui::vbox(
				{
					ftxui::text(textTime) | ftxui::border | ftxui::bold,
					ftxui::text(textScore) | ftxui::border | ftxui::bold,
					ftxui::filler(),
					weaponUI | ftxui::border,
					grenadeUI | ftxui::border,
					healthUI | ftxui::border
				}
			) | ftxui::size(ftxui::WIDTH, ftxui::Constraint::EQUAL, 20)
		}
	);

	// 결과물 반환
	return final;
}

ftxui::Element RenderSystem::RenderCutscene(wstring imageName, wstring textLine1, wstring textLine2, wstring textLine3, wstring textLine4)
{
	// 캔버스 생성
	int canvasWidth = GAME_WIDTH + 40;
	int canvasHeight = GAME_HEIGHT - 20;
	auto canvas = ftxui::Canvas(canvasWidth, canvasHeight);

	// Sprite 로드
	Sprite sprite = ImageLoader::CreateSpriteFromImage(imageName, canvasWidth, canvasHeight);

	// 캔버스에 Sprite 그리기
	for (int y = 0; y < canvasHeight; y++)
	{
		for (int x = 0; x < canvasWidth; x++)
		{
			// Canvas에 색상으로 점(블록)을 찍는다.
			canvas.DrawBlock(x, y, true, sprite.colors[x + (y * canvasWidth)]);
		}
	}

	// 인트로 화면 생성
	auto canvasElement = ftxui::canvas(std::move(canvas));
	auto final = vbox({
				canvasElement | center,
				vbox({
					text(textLine1) | center,
					text(textLine2) | center,
					text(textLine3) | center,
					text(textLine4) | center,
					text(L"스페이스바를 눌러 계속...") | align_right | color(Color::GrayDark)
				}) | border | size(ftxui::WIDTH, Constraint::EQUAL, (GAME_WIDTH + 40) / 2)
		});

	// 결과물 반환
	return final;
}

ftxui::Element RenderSystem::RenderCredits(vector<string> lines, float scrollY)
{
	Elements elements;
	const int startPadding = 10; // 텍스트가 시작되기 전 여백 (화면 아래에서 시작)

	// 타이틀 추가
	elements.push_back(text(L"------------------------------------------------------------------------------------------------")
		| center | color(Color::Red1));
	elements.push_back(text(L"CREDITS") | bold | center | color(ftxui::Color::Red1));
	elements.push_back(text(L""));
	elements.push_back(text(L"Gyeongseong97 - WinAPI + FTXUI + miniaudio C++ Terminal Shooting Game")
		| center | color(Color::Yellow1));
	elements.push_back(text(L"------------------------------------------------------------------------------------------------")
		| center | color(Color::Red1));

	// 현재 스크롤 위치에 따른 렌더링 시작점 계산
	// startPadding에서 scrollY만큼 뺀 값이 현재의 첫 번째 줄 Y 위치
	int currentTopY = startPadding - (int)scrollY;

	// 지나치게 아래로 스크롤하지 못하게 막아준다
	if (currentTopY > 40)
	{
		currentTopY = 40;
		scrollY = -30;
	}

	// 화면 상단보다 아래에 있다면 여백 text을 추가하여 밀어내린다
	if (currentTopY > 0)
	{
		for (int i = 0; i < currentTopY; i++)
		{
			elements.push_back(text(L""));
		}
	}

	// 텍스트 라인 추가
	int linesToSkip = 0;

	// 텍스트가 화면 위로 넘어갔다면(currentTopY < 0), 넘어간 만큼 건너뛰고 렌더링
	if (currentTopY < 0)
	{
		linesToSkip = -currentTopY;
	}

	// 건너뛴 부분부터 화면에 보일 만큼만 렌더링
	for (size_t i = linesToSkip; i < linesToSkip + 40 && i < lines.size(); ++i)
	{
		// 너무 많은 요소를 렌더링하면 성능 저하도 올 수도 있고
		// 어차피 보이지도 않으므로 화면에 보이는 부분까지만 렌더링하면 된다
		// 여기서는 간단하게 40줄 정도 추가 (FTXUI가 화면 밖은 잘라냄)
		elements.push_back(text(lines[i]) | center | color(Color::White));
	}

	// 모든 텍스트가 지나갔을 때의 여백 (자연스러운 종료를 위해)
	if (linesToSkip >= lines.size() + 5)
	{
		elements.push_back(text(L""));
		elements.push_back(text(L"플레이 해주셔서 감사합니다!") | bold | center | color(Color::Yellow));
		elements.push_back(text(L""));
		elements.push_back(text(L"- 심영물 유기하고 게리모드로 빤스런한 월수는 쇼미더심영 후속작을 내놓아라 -") | bold | center | color(Color::Red1));
	}

	return vbox(std::move(elements))
		| size(ftxui::WIDTH, Constraint::EQUAL, (GAME_WIDTH + 40) / 2)
		| size(ftxui::HEIGHT, Constraint::EQUAL, GAME_HEIGHT / 4)
		| center;
}
