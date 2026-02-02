#pragma once

#include <core/rendering/IRenderMode.h>

// Doom-style 2.5D raycasting render mode.
// Renders the scene from first-person perspective using raycasting.
// Sprites become billboards that always face the camera.
class RenderMode25D final : public IRenderMode
{
public:
	void RenderScene(IGraphicsAPI& graphics, BatchArray<IRenderable>& renderables, Camera& camera) override;
	const char* GetName() const override { return "2.5D"; }
};
