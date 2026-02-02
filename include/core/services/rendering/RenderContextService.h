#pragma once

#include <core/containers/BatchArray.h>
#include <core/framework/IService.h>
#include <core/logging/LoggingService.h>
#include <core/rendering/RenderContext.h>
#include <core/rendering/RenderTypes.h>
#include <unordered_map>
#include <memory>
#include <functional>

class TextureService;
class IRenderable;

// Service that manages multiple render contexts.
// Each context represents a window + renderer + camera + render mode combination.
// Supports a primary context (for game logic) and additional debug/observer contexts.
class RenderContextService final : public IService
{
public:
	explicit RenderContextService(LoggingService& logging);
	~RenderContextService();

	// Create a new render context for a window
	// Returns InvalidRenderContextId on failure
	RenderContextId CreateContext(
		WindowId window,
		std::unique_ptr<IGraphicsAPI> graphics,
		std::unique_ptr<IRenderMode> renderMode,
		const Camera& camera,
		const std::string& name = "");

	// Destroy a render context
	void DestroyContext(RenderContextId id);

	// Get a context by ID (returns nullptr if not found)
	RenderContext* GetContext(RenderContextId id);
	const RenderContext* GetContext(RenderContextId id) const;

	// Get context by window ID
	RenderContext* GetContextByWindow(WindowId windowId);

	// Primary context management (used by game logic for canonical camera)
	RenderContext* GetPrimaryContext();
	const RenderContext* GetPrimaryContext() const;
	void SetPrimaryContext(RenderContextId id);
	RenderContextId GetPrimaryContextId() const { return PrimaryContextId; }

	// Check if a context exists
	bool HasContext(RenderContextId id) const;

	// Get the number of contexts
	size_t GetContextCount() const;

	// Iterate over all enabled contexts
	template <typename Func>
	void ForEachContext(Func&& func);

	template <typename Func>
	void ForEachContext(Func&& func) const;

	// Render all contexts
	void RenderAllContexts(BatchArray<IRenderable>& renderables);

	// Called when a window is closed to clean up associated context
	void OnWindowClosed(WindowId windowId);

	// Duplicate textures from one graphics API to another
	// Called when creating new contexts to ensure texture availability
	void SyncTextures(IGraphicsAPI& source, IGraphicsAPI& target, TextureService& textures);

private:
	RenderContextId GenerateContextId();

	std::unordered_map<RenderContextId, std::unique_ptr<RenderContext>> Contexts;
	std::unordered_map<WindowId, RenderContextId> WindowToContext;
	RenderContextId PrimaryContextId = InvalidRenderContextId;
	RenderContextId NextContextId = 1;
	Logger Log;
};

// Template implementations
template <typename Func>
void RenderContextService::ForEachContext(Func&& func)
{
	for (auto& [id, context] : Contexts)
	{
		if (context && context->IsEnabled)
		{
			func(*context);
		}
	}
}

template <typename Func>
void RenderContextService::ForEachContext(Func&& func) const
{
	for (const auto& [id, context] : Contexts)
	{
		if (context && context->IsEnabled)
		{
			func(*context);
		}
	}
}
