#include <core/rendering/RenderableRegistry.h>
#include <core/components/SpriteComponent.h>
#include <algorithm>

void RenderableRegistry::Register(TransformComponent* transform, SpriteComponent* sprite)
{
	// O(1) duplicate check
	if (!RegisteredSprites.insert(sprite).second) {
		return; // Already registered
	}

	Renderables.push_back({ transform, sprite });
	IsDirty = true;
}

void RenderableRegistry::Unregister(SpriteComponent* sprite)
{
	if (RegisteredSprites.erase(sprite) == 0) {
		return; // Wasn't registered
	}

	Renderables.erase(
		std::remove_if(Renderables.begin(), Renderables.end(),
			[sprite](const Renderable& r) { return r.Sprite == sprite; }),
		Renderables.end()
	);
}

void RenderableRegistry::Clear()
{
	Renderables.clear();
	RegisteredSprites.clear();
	IsDirty = false;
}

const std::vector<Renderable>& RenderableRegistry::GetRenderables()
{
	SortIfNeeded();
	return Renderables;
}

void RenderableRegistry::SortIfNeeded()
{
	if (!IsDirty) {
		return;
	}

	std::sort(Renderables.begin(), Renderables.end(),
		[](const Renderable& a, const Renderable& b) {
			int layerA = a.Sprite ? a.Sprite->GetRenderLayer() : 0;
			int layerB = b.Sprite ? b.Sprite->GetRenderLayer() : 0;
			return layerA < layerB;
		});

	IsDirty = false;
}
