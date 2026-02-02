#include <core/world/WorldSystem.h>
#include <core/engine/framework/GameServiceHost.h>
#include <core/world/WorldService.h>
#include <core/gameplay/GameMode.h>
#include <core/gameplay/GameModeService.h>

WorldSystem::WorldSystem(GameServiceHost& services)
	: Services(services)
	, World(services.Get<WorldService>())
{
}

void WorldSystem::Init()
{
	// Build scene via game mode
	if (auto* gameModeService = Services.TryGet<GameModeService>()) {
		if (auto* mode = gameModeService->GetCurrentMode()) {
			mode->Init();
			mode->BuildScene();
		}
	}
}

void WorldSystem::Update()
{
	// Update game mode
	if (auto* gameModeService = Services.TryGet<GameModeService>()) {
		if (auto* mode = gameModeService->GetCurrentMode()) {
			mode->Update();
		}
	}

	// Update camera to follow target
	World.UpdateCameraTarget();
}
