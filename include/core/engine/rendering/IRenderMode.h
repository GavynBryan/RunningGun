#pragma once

#include <core/containers/BatchArray.h>

class IGraphicsAPI;
class IRenderable;
class Camera;

// Abstract interface for rendering strategies.
// Determines HOW the scene is rendered (2D top-down, 2.5D raycaster, etc.)
// Uses IGraphicsAPI for actual draw calls.
class IRenderMode
{
public:
	virtual ~IRenderMode() = default;

	// Render the entire scene
	virtual void RenderScene(IGraphicsAPI& graphics, BatchArray<IRenderable>& renderables, Camera& camera) = 0;

	// Mode identification
	virtual const char* GetName() const = 0;
};
