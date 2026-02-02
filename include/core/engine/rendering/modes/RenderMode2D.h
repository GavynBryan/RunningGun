#pragma once

#include <core/rendering/IRenderMode.h>

// Standard 2D sprite rendering mode.
// Draws sprites at world positions with camera offset.
class RenderMode2D final : public IRenderMode
{
public:
	void RenderScene(IGraphicsAPI& graphics, BatchArray<IRenderable>& renderables, Camera& camera) override;
	const char* GetName() const override { return "2D"; }
};
