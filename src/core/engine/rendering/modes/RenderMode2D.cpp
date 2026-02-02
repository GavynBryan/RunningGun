#include <core/engine/rendering/modes/RenderMode2D.h>
#include <core/engine/rendering/IGraphicsAPI.h>
#include <core/engine/rendering/Camera.h>
#include <core/services/rendering/RenderableRegistry.h>
#include <core/components/SpriteComponent.h>
#include <core/components/TransformComponent.h>

void RenderMode2D::RenderScene(IGraphicsAPI& graphics, RenderableRegistry& registry, Camera& camera)
{
	// Set camera for view transformation
	graphics.SetCamera(camera.GetCameraData());

	// Get all renderables sorted by layer
	const auto& renderables = registry.GetRenderables();

	for (const auto& renderable : renderables) {
		if (!renderable.IsValid()) {
			continue;
		}

		SpriteComponent* sprite = renderable.Sprite;
		TransformComponent* transform = renderable.Transform;

		if (!sprite->IsVisible()) {
			continue;
		}

		// Build render params
		SpriteRenderParams params;
		params.SourceRect = sprite->GetSourceRect();
		
		Vec2 pos = transform->GetPosition();
		Vec2 size = sprite->GetSize();
		params.DestRect = Rectf(pos.x, pos.y, size.x, size.y);
		
		params.FlipX = sprite->GetFlipX();
		params.FlipY = sprite->GetFlipY();
		params.Rotation = transform->GetRotation();
		params.Tint = sprite->GetTint();

		graphics.DrawSprite(sprite->GetTexture(), params);
	}

	// Reset camera after rendering
	graphics.ResetCamera();
}
