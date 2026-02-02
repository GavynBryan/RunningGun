#pragma once

#include <core/entity/Component.h>
#include <core/rendering/TextureHandle.h>
#include <core/math/Rect.h>
#include <core/math/Vec2.h>

class TransformComponent;
class RenderableRegistry;

// Core component for 2D sprite rendering
// Uses TextureHandle (backend-agnostic) instead of SDL_Texture*
// Registers with RenderableRegistry on construction
class SpriteComponent : public ActorComponent
{
public:
	SpriteComponent(Actor& entity, GameServiceHost& services);
	~SpriteComponent() override;

	const char* GetName() const override { return "SpriteComponent"; }

	// Texture
	void SetTexture(TextureHandle texture) { Texture = texture; }
	TextureHandle GetTexture() const { return Texture; }

	// Source rectangle (for sprite sheets/animation)
	void SetSourceRect(const Recti& rect);
	void SetSourceRect(int x, int y, int width, int height);
	const Recti& GetSourceRect() const { return SourceRect; }
	bool HasSourceRect() const { return HasSrcRect; }
	void ClearSourceRect() { HasSrcRect = false; }

	// Size (destination dimensions)
	void SetSize(const Vec2& size) { Size = size; }
	void SetSize(float width, float height) { Size.x = width; Size.y = height; }
	const Vec2& GetSize() const { return Size; }
	float GetWidth() const { return Size.x; }
	float GetHeight() const { return Size.y; }

	// Flip
	void SetFlipX(bool flip) { FlipX = flip; }
	void SetFlipY(bool flip) { FlipY = flip; }
	bool GetFlipX() const { return FlipX; }
	bool GetFlipY() const { return FlipY; }

	// Render layer (for draw ordering)
	void SetRenderLayer(int layer) { RenderLayer = layer; }
	int GetRenderLayer() const { return RenderLayer; }

	// Visibility
	void SetVisible(bool visible) { Visible = visible; }
	bool IsVisible() const { return Visible; }

	// Origin/pivot point (relative to sprite, 0-1 range)
	void SetOrigin(const Vec2& origin) { Origin = origin; }
	void SetOrigin(float x, float y) { Origin.x = x; Origin.y = y; }
	const Vec2& GetOrigin() const { return Origin; }

	// Get bounds in world space (needs position from TransformComponent)
	Rectf GetLocalBounds() const;

	// Access to paired transform (cached on Start)
	TransformComponent* GetTransform() const { return PairedTransform; }

private:
	RenderableRegistry* Registry = nullptr;
	TransformComponent* PairedTransform = nullptr;
	TextureHandle Texture;
	Recti SourceRect = Recti(0, 0, 0, 0);
	Vec2 Size = Vec2(0.0f, 0.0f);
	Vec2 Origin = Vec2(0.0f, 0.0f);
	int RenderLayer = 0;
	bool HasSrcRect = false;
	bool FlipX = false;
	bool FlipY = false;
	bool Visible = true;
};
