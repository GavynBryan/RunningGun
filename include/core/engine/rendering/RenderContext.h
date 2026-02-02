#pragma once

#include <core/platform/IPlatformBackend.h>
#include <core/rendering/Camera.h>
#include <memory>
#include <cstdint>

class IGraphicsAPI;
class IRenderMode;

// Unique identifier for render contexts
using RenderContextId = uint32_t;
constexpr RenderContextId InvalidRenderContextId = 0;

// A render context bundles everything needed to render to a specific window:
// - Window target
// - Graphics API (renderer) for that window
// - Camera (view into the world)
// - Render mode (rendering strategy: 2D, wireframe, etc.)
//
// Each context is independent and can have different camera positions,
// zoom levels, and render modes - useful for debug views, minimaps, etc.
struct RenderContext
{
	RenderContextId Id = InvalidRenderContextId;
	WindowId Window = InvalidWindowId;
	
	// Owned resources
	std::unique_ptr<IGraphicsAPI> Graphics;
	std::unique_ptr<IRenderMode> RenderMode;
	Camera ContextCamera;
	
	// Context metadata
	std::string Name;  // For debugging/identification (e.g., "Main", "Debug View 1")
	bool IsEnabled = true;  // Can be disabled without destroying

	RenderContext() = default;
	~RenderContext() = default;

	// Move-only (owns unique_ptrs)
	RenderContext(RenderContext&&) = default;
	RenderContext& operator=(RenderContext&&) = default;
	RenderContext(const RenderContext&) = delete;
	RenderContext& operator=(const RenderContext&) = delete;
};
