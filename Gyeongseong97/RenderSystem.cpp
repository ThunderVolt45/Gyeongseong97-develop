#include "RenderSystem.h"
#include "GameManager.h"
#include "GameConstants.h"

std::string RenderSystem::errorMessage = "";
bool RenderSystem::isErrorActive = false;
long long RenderSystem::errorDisplayStartTime = 0;

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

ftxui::Element RenderSystem::Render()
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

		// 플레이어 오브젝트 그리기
		DrawObjectSprite(canvas, gameManager.player);

		// 게임 오브젝트 그리기
		for (const auto& object : gameManager.gameObjects)
		{
			DrawObjectSprite(canvas, *object);
		}
	}

	// FPS 표시
	canvas.DrawText(0, 0, 
		"FPS : " + std::to_string(gameManager.currentFps) 
		+ ", Logic : " + std::to_string(gameManager.currentLps));

	// 게임 오버 시 게임 오버 메시지 표시
	if (gameManager.IsGameOver)
	{
		canvas.DrawText(GAME_WIDTH / 2 - (5 * 2) - 2, GAME_HEIGHT / 2 - 8, 
			"GAME OVER!",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::Black;
				p.background_color = ftxui::Color::Red;
			});
		canvas.DrawText(GAME_WIDTH / 2 - (22 * 2) - 2, GAME_HEIGHT / 2, 
			"김두한은 오렌지병이었던 고혈압으로 쓰러졌다.",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::White;
			});
		canvas.DrawText(GAME_WIDTH / 2 - (18 * 2) - 2, GAME_HEIGHT / 2 + 8, 
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
		canvas.DrawText(GAME_WIDTH / 2 - (25 * 2) - 2, GAME_HEIGHT / 2, 
			"김두한은 사악한 공산당원들을 모조리 다 쓸어버렸다!",
			[](ftxui::Pixel& p) {
				p.foreground_color = ftxui::Color::White;
			});
		canvas.DrawText(GAME_WIDTH / 2 - (18 * 2) - 2, GAME_HEIGHT / 2 + 8, 
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

	// 렌더링 결과 생성
	auto finalRender = ftxui::hbox(
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

	return finalRender;
}