#include <core/engine/WorldService.h>
#include <core/GameMode.h>
#include <core/World.h>
#include <core/engine/RunnerService.h>
#include <core/engine/TimerService.h>
#include <cassert>

void WorldService::Init()
{
	if (!WorldContext) {
		WorldContext = std::make_unique<World>(GetHost());
	}
	WorldContext->SetGameMode(Mode);
	WorldContext->Init();
	if (Mode) {
		Mode->Init();
	}
	WorldContext->BuildScene();
	SceneInitialized = true;
}

void WorldService::Update()
{
	assert(WorldContext);

	if (!SceneInitialized) {
		Init();
	}

	WorldContext->Start();
	if (Mode) {
		Mode->Update();
	}
	WorldContext->Update();
	WorldContext->PostUpdate();
	if (Mode) {
		Mode->PostUpdate();
	}
	WorldContext->Render();
}

void WorldService::SetGameMode(GameMode* mode)
{
	Mode = mode;
	if (WorldContext) {
		WorldContext->SetGameMode(Mode);
	}
}

World& WorldService::GetWorld() const
{
	assert(WorldContext);
	return *WorldContext;
}

void WorldService::ResetScene()
{
	SceneInitialized = false;
	GetHost().Get<TimerService>().Reset();
	GetHost().Get<RunnerService>().ResetClock();
	if (WorldContext) {
		WorldContext->ClearEntities();
	}
}
