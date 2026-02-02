#pragma once

#include <core/containers/BatchArray.h>
#include <core/framework/ISystem.h>

class GameServiceHost;
class RenderContextService;
class IRenderable;

// System that triggers rendering each frame.
// Iterates all render contexts and renders each with its own camera/render mode.
class RenderSystem final : public ISystem
{
public:
	explicit RenderSystem(GameServiceHost& services);

	void Init() override;
	void Update() override;
	void Shutdown() override;

private:
	GameServiceHost& Services;
	RenderContextService* CachedContextService = nullptr;
	BatchArray<IRenderable>* CachedRenderables = nullptr;
};
