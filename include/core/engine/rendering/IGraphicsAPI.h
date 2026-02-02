#pragma once

#include <core/rendering/RenderTypes.h>
#include <core/rendering/TextureHandle.h>
#include <string>

// Abstract interface for graphics API wrappers.
// This is a low-level abstraction over platform graphics APIs (SDL, OpenGL, Vulkan, etc.)
// NOT responsible for rendering logic - just provides drawing primitives.
// Rendering strategies (2D, 2.5D raycaster) are handled by IRenderMode implementations.
class IGraphicsAPI
{
public:
	virtual ~IGraphicsAPI() = default;

	// Lifecycle
	virtual bool Init() = 0;
	virtual void Shutdown() = 0;

	// Frame management
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void Clear(const Color& color) = 0;
	virtual void Present() = 0;

	// Texture management
	virtual bool UploadTexture(TextureHandle handle, const TextureData& data) = 0;
	virtual bool UploadTextureFromFile(TextureHandle handle, const std::string& path) = 0;
	virtual void DestroyTexture(TextureHandle handle) = 0;
	virtual Vec2 GetTextureSize(TextureHandle handle) const = 0;

	// Drawing operations
	virtual void DrawSprite(TextureHandle texture, const SpriteRenderParams& params) = 0;
	virtual void DrawRect(const Rectf& rect, const Color& color, bool filled = true) = 0;
	virtual void DrawLine(const Vec2& start, const Vec2& end, const Color& color) = 0;

	// Camera/view transformation
	virtual void SetCamera(const CameraData& camera) = 0;
	virtual void ResetCamera() = 0;

	// Backend info
	virtual const char* GetName() const = 0;
};
