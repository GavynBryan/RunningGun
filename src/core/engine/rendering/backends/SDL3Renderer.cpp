#include <core/engine/rendering/backends/SDL3Renderer.h>
#include <SDL3_image/SDL_image.h>

SDL3Renderer::SDL3Renderer(SDL_Window* window)
	: Window(window)
{
}

SDL3Renderer::~SDL3Renderer()
{
	Shutdown();
}

bool SDL3Renderer::Init()
{
	if (!Window) {
		return false;
	}

	SDLRenderer = SDL_CreateRenderer(Window, nullptr);
	if (!SDLRenderer) {
		SDL_Log("Failed to create SDL renderer: %s", SDL_GetError());
		return false;
	}

	return true;
}

void SDL3Renderer::Shutdown()
{
	// Destroy all textures
	for (auto& [handle, texture] : Textures) {
		if (texture) {
			SDL_DestroyTexture(texture);
		}
	}
	Textures.clear();

	if (SDLRenderer) {
		SDL_DestroyRenderer(SDLRenderer);
		SDLRenderer = nullptr;
	}
}

void SDL3Renderer::BeginFrame()
{
	// Nothing to do - SDL3 handles this automatically
}

void SDL3Renderer::EndFrame()
{
	// Nothing to do - Present is called separately
}

void SDL3Renderer::Clear(const Color& color)
{
	SDL_SetRenderDrawColor(SDLRenderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(SDLRenderer);
}

void SDL3Renderer::Present()
{
	SDL_RenderPresent(SDLRenderer);
}

bool SDL3Renderer::UploadTexture(TextureHandle handle, const TextureData& data)
{
	if (!SDLRenderer || data.Pixels.empty()) {
		return false;
	}

	// Create SDL surface from pixel data
	SDL_Surface* surface = SDL_CreateSurfaceFrom(
		data.Width, data.Height,
		SDL_PIXELFORMAT_RGBA32,
		const_cast<uint8_t*>(data.Pixels.data()),
		data.Width * data.Channels
	);

	if (!surface) {
		SDL_Log("Failed to create surface: %s", SDL_GetError());
		return false;
	}

	// Create texture from surface
	SDL_Texture* texture = SDL_CreateTextureFromSurface(SDLRenderer, surface);
	SDL_DestroySurface(surface);

	if (!texture) {
		SDL_Log("Failed to create texture: %s", SDL_GetError());
		return false;
	}

	// Replace existing texture if any
	DestroyTexture(handle);
	Textures[handle] = texture;
	return true;
}

bool SDL3Renderer::UploadTextureFromFile(TextureHandle handle, const std::string& path)
{
	if (!SDLRenderer) {
		return false;
	}

	SDL_Texture* texture = IMG_LoadTexture(SDLRenderer, path.c_str());
	if (!texture) {
		SDL_Log("Failed to load texture '%s': %s", path.c_str(), SDL_GetError());
		return false;
	}

	// Replace existing texture if any
	DestroyTexture(handle);
	Textures[handle] = texture;
	return true;
}

void SDL3Renderer::DestroyTexture(TextureHandle handle)
{
	auto it = Textures.find(handle);
	if (it != Textures.end()) {
		if (it->second) {
			SDL_DestroyTexture(it->second);
		}
		Textures.erase(it);
	}
}

Vec2 SDL3Renderer::GetTextureSize(TextureHandle handle) const
{
	auto it = Textures.find(handle);
	if (it == Textures.end() || !it->second) {
		return Vec2(0.0f, 0.0f);
	}

	float w, h;
	SDL_GetTextureSize(it->second, &w, &h);
	return Vec2(w, h);
}

void SDL3Renderer::DrawSprite(TextureHandle texture, const SpriteRenderParams& params)
{
	auto it = Textures.find(texture);
	if (it == Textures.end() || !it->second) {
		return;
	}

	SDL_FRect srcRect = {
		params.SourceRect.x, params.SourceRect.y,
		params.SourceRect.width, params.SourceRect.height
	};
	
	SDL_FRect dstRect = TransformRect(params.DestRect);

	// Determine flip mode
	SDL_FlipMode flip = SDL_FLIP_NONE;
	if (params.FlipX && params.FlipY) {
		flip = static_cast<SDL_FlipMode>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
	} else if (params.FlipX) {
		flip = SDL_FLIP_HORIZONTAL;
	} else if (params.FlipY) {
		flip = SDL_FLIP_VERTICAL;
	}

	// Set color mod for tint
	SDL_SetTextureColorMod(it->second, params.Tint.r, params.Tint.g, params.Tint.b);
	SDL_SetTextureAlphaMod(it->second, params.Tint.a);

	if (params.Rotation != 0.0f || flip != SDL_FLIP_NONE) {
		SDL_FPoint center = { params.Origin.x, params.Origin.y };
		SDL_RenderTextureRotated(SDLRenderer, it->second, &srcRect, &dstRect,
		                         params.Rotation, &center, flip);
	} else {
		SDL_RenderTexture(SDLRenderer, it->second, &srcRect, &dstRect);
	}
}

void SDL3Renderer::DrawRect(const Rectf& rect, const Color& color, bool filled)
{
	SDL_SetRenderDrawColor(SDLRenderer, color.r, color.g, color.b, color.a);
	SDL_FRect sdlRect = TransformRect(rect);

	if (filled) {
		SDL_RenderFillRect(SDLRenderer, &sdlRect);
	} else {
		SDL_RenderRect(SDLRenderer, &sdlRect);
	}
}

void SDL3Renderer::DrawLine(const Vec2& start, const Vec2& end, const Color& color)
{
	SDL_SetRenderDrawColor(SDLRenderer, color.r, color.g, color.b, color.a);
	
	Vec2 screenStart = start;
	Vec2 screenEnd = end;
	
	if (HasCamera) {
		screenStart.x -= CurrentCamera.Position.x - CurrentCamera.ViewSize.x * 0.5f;
		screenStart.y -= CurrentCamera.Position.y - CurrentCamera.ViewSize.y * 0.5f;
		screenEnd.x -= CurrentCamera.Position.x - CurrentCamera.ViewSize.x * 0.5f;
		screenEnd.y -= CurrentCamera.Position.y - CurrentCamera.ViewSize.y * 0.5f;
	}
	
	SDL_RenderLine(SDLRenderer, screenStart.x, screenStart.y, screenEnd.x, screenEnd.y);
}

void SDL3Renderer::SetCamera(const CameraData& camera)
{
	CurrentCamera = camera;
	HasCamera = true;
}

void SDL3Renderer::ResetCamera()
{
	HasCamera = false;
	CurrentCamera = CameraData();
}

SDL_Texture* SDL3Renderer::GetNativeTexture(TextureHandle handle) const
{
	auto it = Textures.find(handle);
	return (it != Textures.end()) ? it->second : nullptr;
}

SDL_FRect SDL3Renderer::TransformRect(const Rectf& worldRect) const
{
	SDL_FRect result;
	
	if (HasCamera) {
		float offsetX = CurrentCamera.Position.x - CurrentCamera.ViewSize.x * 0.5f;
		float offsetY = CurrentCamera.Position.y - CurrentCamera.ViewSize.y * 0.5f;
		result.x = worldRect.x - offsetX;
		result.y = worldRect.y - offsetY;
	} else {
		result.x = worldRect.x;
		result.y = worldRect.y;
	}
	
	result.w = worldRect.width;
	result.h = worldRect.height;
	return result;
}
