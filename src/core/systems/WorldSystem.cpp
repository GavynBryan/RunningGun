#include <core/systems/WorldSystem.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/WorldService.h>
#include <core/engine/TimerService.h>
#include <core/engine/RunnerService.h>
#include <core/GameMode.h>
#include <core/World.h>

WorldSystem::WorldSystem(GameServiceHost& services)
	: Services(services)
{
}

void WorldSystem::Init()
{
	InitScene();
}

void WorldSystem::InitScene()
{
	auto& worldService = Services.Get<WorldService>();
	auto& world = worldService.GetWorld();
	auto* mode = worldService.GetGameMode();

	world.Init();
	if (mode) {
		mode->Init();
	}
	world.BuildScene();
	worldService.SetSceneInitialized(true);
}

void WorldSystem::Update()
{
	auto& worldService = Services.Get<WorldService>();
	auto& world = worldService.GetWorld();
	auto* mode = worldService.GetGameMode();

	if (!worldService.IsSceneInitialized()) {
		InitScene();
	}

	world.Start();
	if (mode) {
		mode->Update();
	}
	world.Update();
	world.PostUpdate();
	if (mode) {
		mode->PostUpdate();
	}
	world.Render();
}

void WorldSystem::ResetScene()
{
	auto& worldService = Services.Get<WorldService>();
	auto& world = worldService.GetWorld();

	worldService.SetSceneInitialized(false);
	Services.Get<TimerService>().Reset();
	Services.Get<RunnerService>().ResetClock();
	world.ClearEntities();
}
