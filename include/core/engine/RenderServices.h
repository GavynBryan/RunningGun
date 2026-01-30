#pragma once

#include <memory>

class Camera;
class ResourceHandler;
class EngineServices;

class RenderServices
{
public:
	ResourceHandler& GetTextureHandler() const;
	Camera& GetCamera() const;

private:
	std::unique_ptr<ResourceHandler> TextureHandler;
	std::unique_ptr<Camera> CameraContext;

	void SetTextureHandler(std::unique_ptr<ResourceHandler> _handler);
	void SetCamera(std::unique_ptr<Camera> _camera);

	friend class EngineServices;
};
