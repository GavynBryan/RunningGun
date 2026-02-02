#pragma once

#include <core/framework/IService.h>
#include <vector>
#include <unordered_set>

class SpriteComponent;
class TransformComponent;

// Renderable entry that combines transform and sprite for rendering
struct Renderable
{
	TransformComponent* Transform = nullptr;
	SpriteComponent* Sprite = nullptr;

	bool IsValid() const { return Transform != nullptr && Sprite != nullptr; }
};

// Service that maintains a registry of all renderable sprite components.
// Components register/unregister themselves, decoupling RenderSystem from Entity iteration.
// Maintains sorted order by render layer for efficient rendering.
class RenderableRegistry final : public IService
{
public:
	void Register(TransformComponent* transform, SpriteComponent* sprite);
	void Unregister(SpriteComponent* sprite);

	// Call when a sprite's render layer changes
	void MarkDirty() { IsDirty = true; }

	// Returns renderables sorted by layer (re-sorts only if dirty)
	const std::vector<Renderable>& GetRenderables();

	void Clear();

private:
	void SortIfNeeded();

	std::vector<Renderable> Renderables;
	std::unordered_set<SpriteComponent*> RegisteredSprites; // O(1) duplicate check
	bool IsDirty = false;
};
