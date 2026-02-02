#include <core/engine/rendering/modes/RenderMode25D.h>
#include <core/engine/rendering/IGraphicsAPI.h>
#include <core/engine/rendering/Camera.h>
#include <core/rendering/IRenderable.h>
#include <cmath>
#include <algorithm>
#include <vector>

void RenderMode25D::RenderScene(IGraphicsAPI& graphics, BatchArray<IRenderable>& renderables, Camera& camera)
{
	// 2.5D raycaster rendering - simplified billboard sprite rendering
	// This is a basic implementation - a full raycaster would include wall rendering

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
		const IRenderable* renderable;
		float distance;
		float screenX;
		float scale;
	};
	std::vector<SpriteToDraw> spritesToDraw;

	for (const auto* renderable : renderables) {
		if (!renderable || !renderable->IsVisible()) {
			continue;
		}

		Transform2D transform = renderable->GetTransform();
		
		// Relative position to camera
		float relX = transform.Position.x - camPos.x;
		float relY = transform.Position.y - camPos.y;

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
			renderable,
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
		const IRenderable* renderable = sprite.renderable;
		Vec2 spriteSize = renderable->GetSize();

		float spriteWidth = spriteSize.x * sprite.scale;
		float spriteHeight = spriteSize.y * sprite.scale;

		float drawX = sprite.screenX - spriteWidth / 2.0f;
		float drawY = (viewSize.y - spriteHeight) / 2.0f;

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
		params.DestRect = Rectf(drawX, drawY, spriteWidth, spriteHeight);
		params.FlipX = renderable->GetFlipX();
		params.FlipY = renderable->GetFlipY();

		graphics.DrawSprite(renderable->GetTexture(), params);
	}
}
