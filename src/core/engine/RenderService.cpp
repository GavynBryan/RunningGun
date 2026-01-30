#include <core/engine/RenderService.h>
#include <core/Camera.h>
#include <core/ResourceHandler.h>
#include <cassert>
#include <utility>

RenderService::RenderService(SDL_Renderer* renderer, std::unique_ptr<ResourceHandler> handler, std::unique_ptr<Camera> camera)
	: Renderer(renderer),
	TextureHandler(std::move(handler)),
	CameraContext(std::move(camera))
{
}

ResourceHandler& RenderService::GetTextureHandler() const
{
	assert(TextureHandler);
	return *TextureHandler;
}

Camera& RenderService::GetCamera() const
{
	assert(CameraContext);
	return *CameraContext;
}
