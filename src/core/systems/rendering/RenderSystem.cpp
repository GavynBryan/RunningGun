#include <core/rendering/RenderSystem.h>
#include <core/rendering/RenderContextService.h>
#include <core/rendering/RenderableRegistry.h>
#include <core/framework/GameServiceHost.h>

RenderSystem::RenderSystem(GameServiceHost& services)
	: Services(services)
{
}

void RenderSystem::Init()
{
	CachedContextService = Services.TryGet<RenderContextService>();
	CachedRegistry = Services.TryGet<RenderableRegistry>();
}

void RenderSystem::Update()
{
	if (!CachedContextService || !CachedRegistry) {
		return;
	}

	// Render all contexts (each with its own camera and render mode)
	CachedContextService->RenderAllContexts(*CachedRegistry);
}

void RenderSystem::Shutdown()
{
}
