#include <core/rendering/RenderableRegistry.h>
#include <core/rendering/IRenderable.h>
#include <algorithm>

void RenderableRegistry::RegisterComponent(ActorComponent* component)
{
	Register(dynamic_cast<IRenderable*>(component));
}

void RenderableRegistry::UnregisterComponent(ActorComponent* component)
{
	Unregister(dynamic_cast<IRenderable*>(component));
}

void RenderableRegistry::Register(IRenderable* renderable)
{
	if (!renderable)
	{
		return;
	}

	// O(1) duplicate check via index map
	if (IndexMap.find(renderable) != IndexMap.end())
	{
		return; // Already registered
	}

	size_t index = Renderables.size();
	Renderables.push_back(renderable);
	IndexMap[renderable] = index;
	IsDirty = true;
}

void RenderableRegistry::Unregister(IRenderable* renderable)
{
	if (!renderable)
	{
		return;
	}

	auto it = IndexMap.find(renderable);
	if (it == IndexMap.end())
	{
		return; // Wasn't registered
	}

	size_t indexToRemove = it->second;
	size_t lastIndex = Renderables.size() - 1;

	// Swap with last element if not already last (swap-and-pop for O(1) removal)
	if (indexToRemove != lastIndex)
	{
		Renderables[indexToRemove] = Renderables[lastIndex];
		IndexMap[Renderables[indexToRemove]] = indexToRemove;
	}

	Renderables.pop_back();
	IndexMap.erase(it);
	IsDirty = true;  // Order may have changed
}

bool RenderableRegistry::Contains(IRenderable* renderable) const
{
	return IndexMap.find(renderable) != IndexMap.end();
}

void RenderableRegistry::Clear()
{
	Renderables.clear();
	IndexMap.clear();
	IsDirty = false;
}

const std::vector<IRenderable*>& RenderableRegistry::GetRenderables()
{
	SortIfNeeded();
	return Renderables;
}

void RenderableRegistry::SortIfNeeded()
{
	if (!IsDirty)
	{
		return;
	}

	std::sort(Renderables.begin(), Renderables.end(),
		[](const IRenderable* a, const IRenderable* b) {
			int layerA = a ? a->GetRenderLayer() : 0;
			int layerB = b ? b->GetRenderLayer() : 0;
			return layerA < layerB;
		});

	// Rebuild index map after sort
	for (size_t i = 0; i < Renderables.size(); ++i)
	{
		IndexMap[Renderables[i]] = i;
	}

	IsDirty = false;
}
