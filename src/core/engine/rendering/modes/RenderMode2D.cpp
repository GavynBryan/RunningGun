#include <core/engine/rendering/modes/RenderMode2D.h>
#include <core/engine/rendering/IGraphicsAPI.h>
#include <core/engine/rendering/Camera.h>
#include <core/services/rendering/RenderableRegistry.h>
#include <core/rendering/IRenderable.h>

void RenderMode2D::RenderScene(IGraphicsAPI& graphics, RenderableRegistry& registry, Camera& camera)
{
	// Set camera for view transformation
	graphics.SetCamera(camera.GetCameraData());

	// Get all renderables sorted by layer
	const auto& renderables = registry.GetRenderables();

	for (const auto* renderable : renderables) {
		if (!renderable || !renderable->IsVisible()) {
			continue;
		}

		Transform2D transform = renderable->GetTransform();

		// Build render params
		SpriteRenderParams params;
		
		if (renderable->HasSourceRect()) {
			const auto& src = renderable->GetSourceRect();
			params.SourceRect = Rectf(
				static_cast<float>(src.x),
				static_cast<float>(src.y),
				static_cast<float>(src.width),
				static_cast<float>(src.height)
			);
		}
		
		const Vec2& size = renderable->GetSize();
		params.DestRect = Rectf(
			transform.Position.x,
			transform.Position.y,
			size.x * transform.Scale.x,
			size.y * transform.Scale.y
		);
		
		params.FlipX = renderable->GetFlipX();
		params.FlipY = renderable->GetFlipY();
		params.Rotation = transform.Rotation;
		params.Origin = renderable->GetOrigin();

		graphics.DrawSprite(renderable->GetTexture(), params);
	}

	// Reset camera after rendering
	graphics.ResetCamera();
}
