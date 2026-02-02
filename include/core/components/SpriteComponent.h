#pragma once

#include <core/entity/Component.h>
#include <core/rendering/IRenderable.h>

class IBatchArray;

//=============================================================================
// SpriteComponent
// 
// Core component for 2D sprite rendering. Implements IRenderable interface.
// Uses TextureHandle (backend-agnostic) instead of SDL_Texture*.
// Registers with BatchArray<IRenderable> on construction (auto-removes via RAII).
//=============================================================================
class SpriteComponent : public ActorComponent, public IRenderable
{
public:
	SpriteComponent(Actor& entity, GameServiceHost& services);
	~SpriteComponent() override = default;

	const char* GetName() const override { return "SpriteComponent"; }

	//=========================================================================
	// IRenderable interface
	//=========================================================================
	Transform2D GetTransform() const override;
	TextureHandle GetTexture() const override { return Texture; }
	const Recti& GetSourceRect() const override { return SourceRect; }
	bool HasSourceRect() const override { return HasSrcRect; }
	const Vec2& GetSize() const override { return Size; }
	bool GetFlipX() const override { return FlipX; }
	bool GetFlipY() const override { return FlipY; }
	int GetRenderLayer() const override { return RenderLayer; }
	bool IsVisible() const override { return Visible; }
	const Vec2& GetOrigin() const override { return Origin; }

	//=========================================================================
	// Setters (not part of IRenderable)
	//=========================================================================
	void SetTexture(TextureHandle texture) { Texture = texture; }

	// Source rectangle (for sprite sheets/animation)
	void SetSourceRect(const Recti& rect);
	void SetSourceRect(int x, int y, int width, int height);
	void ClearSourceRect() { HasSrcRect = false; }

	// Size (destination dimensions)
	void SetSize(const Vec2& size) { Size = size; }
	void SetSize(float width, float height) { Size.x = width; Size.y = height; }
	float GetWidth() const { return Size.x; }
	float GetHeight() const { return Size.y; }

	// Flip
	void SetFlipX(bool flip) { FlipX = flip; }
	void SetFlipY(bool flip) { FlipY = flip; }

	// Render layer (for draw ordering)
	void SetRenderLayer(int layer);

	// Visibility
	void SetVisible(bool visible) { Visible = visible; }

	// Origin/pivot point (relative to sprite, 0-1 range)
	void SetOrigin(const Vec2& origin) { Origin = origin; }
	void SetOrigin(float x, float y) { Origin.x = x; Origin.y = y; }

	// Get bounds in local space
	Rectf GetLocalBounds() const;

private:
	IBatchArray* BatchArray = nullptr;  // For MarkDirty() calls
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
