#pragma once

#include <core/rendering/IGraphicsAPI.h>
#include <SDL3/SDL.h>
#include <unordered_map>

// SDL3-based renderer implementation.
// Wraps SDL3 rendering calls - does NOT contain rendering logic.
// Creates and owns the SDL_Renderer for a given window.
class SDL3Renderer final : public IGraphicsAPI
{
public:
	// Creates an SDL_Renderer for the given window
	explicit SDL3Renderer(SDL_Window* window);
	~SDL3Renderer() override;

	// Lifecycle
	bool Init() override;
	void Shutdown() override;

	// Frame management
	void BeginFrame() override;
	void EndFrame() override;
	void Clear(const Color& color) override;
	void Present() override;

	// Texture management
	bool UploadTexture(TextureHandle handle, const TextureData& data) override;
	bool UploadTextureFromFile(TextureHandle handle, const std::string& path) override;
	void DestroyTexture(TextureHandle handle) override;
	Vec2 GetTextureSize(TextureHandle handle) const override;

	// Drawing
	void DrawSprite(TextureHandle texture, const SpriteRenderParams& params) override;
	void DrawRect(const Rectf& rect, const Color& color, bool filled = true) override;
	void DrawLine(const Vec2& start, const Vec2& end, const Color& color) override;

	// Camera
	void SetCamera(const CameraData& camera) override;
	void ResetCamera() override;

	// Backend info
	const char* GetName() const override { return "SDL3"; }

	// SDL-specific access (for legacy code during migration)
	SDL_Renderer* GetSDLRenderer() const { return SDLRenderer; }
	SDL_Texture* GetNativeTexture(TextureHandle handle) const;

private:
	SDL_FRect TransformRect(const Rectf& worldRect) const;

	SDL_Window* Window = nullptr;       // Not owned - managed by PlatformService
	SDL_Renderer* SDLRenderer = nullptr; // Owned - created and destroyed by this class
	std::unordered_map<TextureHandle, SDL_Texture*> Textures;

	// Current camera state
	CameraData CurrentCamera;
	bool HasCamera = false;
};
