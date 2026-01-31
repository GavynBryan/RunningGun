#include <core/engine/WorldService.h>
#include <core/World.h>
#include <cassert>

WorldService::WorldService(GameServiceHost& services)
	: WorldContext(std::make_unique<World>(services))
{
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
