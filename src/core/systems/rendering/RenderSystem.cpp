#include <core/rendering/RenderSystem.h>
#include <core/rendering/RenderContextService.h>
#include <core/framework/GameServiceHost.h>

RenderSystem::RenderSystem(GameServiceHost& services)
	: Services(services)
{
}

void RenderSystem::Init()
{
	CachedContextService = Services.TryGet<RenderContextService>();
	CachedRenderables = Services.TryGet<BatchArray<IRenderable>>();
}

void RenderSystem::Update()
{
	if (!CachedContextService || !CachedRenderables) {
		return;
	}

	// Render all contexts (each with its own camera and render mode)
	CachedContextService->RenderAllContexts(*CachedRenderables);
}

void RenderSystem::Shutdown()
{
}
