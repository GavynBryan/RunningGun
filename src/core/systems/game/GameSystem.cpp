#include <core/systems/game/GameSystem.h>
#include <core/services/framework/GameServiceHost.h>
#include <core/services/game/GameModeService.h>
#include <core/gameplay/GameMode.h>

GameSystem::GameSystem(GameServiceHost& services)
	: GameMode(services.Get<GameModeService>())
{
}

void GameSystem::Init()
{
	if (GameMode* mode = GameMode.GetCurrentMode()) {
		mode->Init();
		mode->BuildScene();
	}
}

void GameSystem::Update()
{
	if (GameMode* mode = GameMode.GetCurrentMode()) {
		mode->Update();
	}
}
