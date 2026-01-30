#include <core/engine/EngineServices.h>
#include <core/Entity.h>
#include <core/InputManager.h>
#include <core/Physics.h>
#include <core/ResourceHandler.h>
#include <core/Camera.h>
#include <cassert>
#include <utility>

EngineServices::EngineServices()
	: PhysicsManager(nullptr)
{
}

RenderServices& EngineServices::Render()
{
	return RenderContext;
}

const RenderServices& EngineServices::Render() const
{
	return RenderContext;
}

ResourceHandler& EngineServices::GetTextureHandler() const
{
	return RenderContext.GetTextureHandler();
}

Camera& EngineServices::GetCamera() const
{
	return RenderContext.GetCamera();
}

Physics& EngineServices::GetPhysics() const
{
	assert(PhysicsManager);
	return *PhysicsManager;
}

InputManager& EngineServices::GetInput() const
{
	assert(InputContext);
	return *InputContext;
}

float EngineServices::GetElapsedTime() const
{
	return WorldContext.GetElapsedTime();
}

float EngineServices::DeltaTime() const
{
	return FrameTime.DeltaTime();
}

std::unique_ptr<Entity> EngineServices::CreateEntity(const std::string& _texture, float _width, float _height)
{
	return std::make_unique<Entity>(*this, _texture, _width, _height);
}

void EngineServices::Instantiate(std::unique_ptr<Entity> _entity) const
{
	WorldContext.Instantiate(std::move(_entity));
}

TimerHandle EngineServices::ScheduleTimer(float _delay, std::function<void()> _callback) const
{
	return WorldContext.ScheduleTimer(_delay, _callback);
}

void EngineServices::CancelTimer(TimerHandle _handle) const
{
	WorldContext.CancelTimer(_handle);
}

void EngineServices::Reset() const
{
	WorldContext.Reset();
}

GameMode* EngineServices::GetGameMode() const
{
	return WorldContext.GetGameMode();
}

void EngineServices::SetTextureHandler(std::unique_ptr<ResourceHandler> _handler)
{
	RenderContext.SetTextureHandler(std::move(_handler));
}

void EngineServices::SetPhysics(std::unique_ptr<Physics> _physics)
{
	PhysicsManager = std::move(_physics);
}

void EngineServices::SetCamera(std::unique_ptr<Camera> _camera)
{
	RenderContext.SetCamera(std::move(_camera));
}

void EngineServices::SetWorld(World* _world)
{
	WorldContext.SetWorld(_world);
}

void EngineServices::SetDeltaTime(float _deltaTime)
{
	FrameTime.SetDeltaTime(_deltaTime);
}

void EngineServices::SetInput(InputManager* _input)
{
	InputContext = _input;
}
