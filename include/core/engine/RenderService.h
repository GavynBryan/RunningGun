#pragma once

#include <memory>

class Camera;
class ResourceHandler;
struct SDL_Renderer;

class RenderService final
{
public:
	RenderService(SDL_Renderer* renderer, std::unique_ptr<ResourceHandler> handler, std::unique_ptr<Camera> camera);

	ResourceHandler& GetTextureHandler() const;
	Camera& GetCamera() const;
	SDL_Renderer* GetRenderer() const { return Renderer; }

private:
	SDL_Renderer* Renderer = nullptr;
	std::unique_ptr<ResourceHandler> TextureHandler;
	std::unique_ptr<Camera> CameraContext;
};
