#pragma once

#include <core/math/Transform2D.h>
#include <core/rendering/TextureHandle.h>
#include <core/math/Rect.h>

//=============================================================================
// IRenderable
// 
// Interface for anything that can be rendered by the RenderSystem.
// Decoupled from Actor/Component system - can be implemented by sprites,
// particles, UI elements, or future ECS entities.
//=============================================================================
class IRenderable
{
public:
	virtual ~IRenderable() = default;

	// Transform data for positioning (position, rotation, scale)
	virtual Transform2D GetTransform() const = 0;

	// Texture to render
	virtual TextureHandle GetTexture() const = 0;

	// Source rectangle (sprite sheet region)
	virtual const Recti& GetSourceRect() const = 0;
	virtual bool HasSourceRect() const = 0;

	// Destination size
	virtual const Vec2& GetSize() const = 0;

	// Flip state
	virtual bool GetFlipX() const = 0;
	virtual bool GetFlipY() const = 0;

	// Render ordering
	virtual int GetRenderLayer() const = 0;

	// Visibility
	virtual bool IsVisible() const = 0;

	// Origin/pivot
	virtual const Vec2& GetOrigin() const = 0;
};
