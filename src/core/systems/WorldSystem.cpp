#include <core/systems/WorldSystem.h>
#include <core/services/GameServiceHost.h>
#include <core/services/WorldService.h>

WorldSystem::WorldSystem(GameServiceHost& services)
	: World(services.Get<WorldService>())
{
}

void WorldSystem::Init()
{
	World.BuildScene();
}

void WorldSystem::Update()
{
	// Start newly added entities
	World.StartPendingEntities();
	
	// Process add queue
	World.HandleQueue();
	
	// Update all entities
	World.UpdateEntities();
}
