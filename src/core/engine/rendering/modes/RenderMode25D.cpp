#include <core/engine/rendering/modes/RenderMode25D.h>
#include <core/engine/rendering/IGraphicsAPI.h>
#include <core/engine/rendering/Camera.h>
#include <core/services/rendering/RenderableRegistry.h>
#include <core/components/SpriteComponent.h>
#include <core/components/TransformComponent.h>
#include <cmath>
#include <algorithm>
#include <vector>

void RenderMode25D::RenderScene(IGraphicsAPI& graphics, RenderableRegistry& registry, Camera& camera)
{
	// 2.5D raycaster rendering - simplified billboard sprite rendering
	// This is a basic implementation - a full raycaster would include wall rendering

	const auto& renderables = registry.GetRenderables();

	// Get camera position and direction
	Vec2 camPos = camera.GetPosition();
	float camRotation = camera.GetRotation();
	Vec2 viewSize = camera.GetViewSize();

	// Calculate camera direction vector
	float dirX = std::cos(camRotation * 3.14159f / 180.0f);
	float dirY = std::sin(camRotation * 3.14159f / 180.0f);

	// Calculate perpendicular (camera plane)
	float planeX = -dirY * 0.66f;  // FOV factor
	float planeY = dirX * 0.66f;

	// Collect visible sprites with distance
	struct SpriteToDraw
	{
		const Renderable* renderable;
		float distance;
		float screenX;
		float scale;
	};
	std::vector<SpriteToDraw> spritesToDraw;

	for (const auto& renderable : renderables) {
		if (!renderable.IsValid() || !renderable.Sprite->IsVisible()) {
			continue;
		}

		Vec2 spritePos = renderable.Transform->GetPosition();
		
		// Relative position to camera
		float relX = spritePos.x - camPos.x;
		float relY = spritePos.y - camPos.y;

		// Transform sprite with inverse camera matrix
		float invDet = 1.0f / (planeX * dirY - dirX * planeY);
		float transformX = invDet * (dirY * relX - dirX * relY);
		float transformY = invDet * (-planeY * relX + planeX * relY);

		// Skip sprites behind camera
		if (transformY <= 0.1f) {
			continue;
		}

		// Calculate screen X position
		float screenX = (viewSize.x / 2.0f) * (1.0f + transformX / transformY);

		// Calculate scale based on distance
		float scale = viewSize.y / transformY;

		spritesToDraw.push_back({
			&renderable,
			transformY,
			screenX,
			scale
		});
	}

	// Sort by distance (far to near) for painter's algorithm
	std::sort(spritesToDraw.begin(), spritesToDraw.end(),
		[](const SpriteToDraw& a, const SpriteToDraw& b) {
			return a.distance > b.distance;
		}
	);

	// Render sprites
	for (const auto& sprite : spritesToDraw) {
		SpriteComponent* spriteComp = sprite.renderable->Sprite;
		Vec2 spriteSize = spriteComp->GetSize();

		float spriteWidth = spriteSize.x * sprite.scale;
		float spriteHeight = spriteSize.y * sprite.scale;

		float drawX = sprite.screenX - spriteWidth / 2.0f;
		float drawY = (viewSize.y - spriteHeight) / 2.0f;

		SpriteRenderParams params;
		params.SourceRect = spriteComp->GetSourceRect();
		params.DestRect = Rectf(drawX, drawY, spriteWidth, spriteHeight);
		params.FlipX = spriteComp->GetFlipX();
		params.FlipY = spriteComp->GetFlipY();
		params.Tint = spriteComp->GetTint();

		graphics.DrawSprite(spriteComp->GetTexture(), params);
	}
}
