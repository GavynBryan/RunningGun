#include <core/engine/RenderServices.h>
#include <core/ResourceHandler.h>
#include <core/Camera.h>
#include <cassert>
#include <utility>

void RenderServices::SetTextureHandler(std::unique_ptr<ResourceHandler> _handler)
{
	TextureHandler = std::move(_handler);
}

void RenderServices::SetCamera(std::unique_ptr<Camera> _camera)
{
	CameraContext = std::move(_camera);
}

ResourceHandler& RenderServices::GetTextureHandler() const
{
	assert(TextureHandler);
	return *TextureHandler;
}

Camera& RenderServices::GetCamera() const
{
	assert(CameraContext);
	return *CameraContext;
}
