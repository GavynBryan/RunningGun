#pragma once

#include <core/containers/BatchArray.h>
#include <vector>
#include <unordered_map>

class IRenderable;

//=============================================================================
// RenderableRegistry
// 
// Batch array for IRenderable components.
// Stores renderables contiguously for cache-friendly iteration and supports
// layer-based sorting for correct draw ordering.
// 
// Components register via TryRegisterToBatchArray<IRenderable>().
// Uses swap-and-pop for O(1) removal.
//=============================================================================
class RenderableRegistry final : public IBatchArray, public IService
{
public:
	// IBatchArray interface
	void Add(void* item) override;
	void Remove(void* item) override;
	
	// Direct register/unregister (typed)
	void Register(IRenderable* renderable);
	void Unregister(IRenderable* renderable);

	// Call when a renderable's render layer changes
	void MarkDirty() { IsDirty = true; }

	// Returns renderables sorted by layer (re-sorts only if dirty)
	const std::vector<IRenderable*>& GetRenderables();

	// Check if a renderable is registered
	bool Contains(IRenderable* renderable) const;
	
	// Get count of registered renderables
	size_t Count() const { return Renderables.size(); }

	void Clear();

private:
	void SortIfNeeded();

	std::vector<IRenderable*> Renderables;
	std::unordered_map<IRenderable*, size_t> IndexMap;  // O(1) lookup for swap-and-pop
	bool IsDirty = false;
};
