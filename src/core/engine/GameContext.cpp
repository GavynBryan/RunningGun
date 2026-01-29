#include <core/GameContext.h>
#include <core/World.h>
#include <cassert>

GameContext::GameContext()
	: WorldContext(nullptr)
	, DeltaTimeValue(0)
{
}

GameContext::~GameContext()
{
}

void GameContext::SetTextureHandler(std::unique_ptr<ResourceHandler> _handler)
{
	TextureHandler = std::move(_handler);
}

void GameContext::SetPhysics(std::unique_ptr<Physics> _physics)
{
	PhysicsManager = std::move(_physics);
}

void GameContext::SetCamera(std::unique_ptr<Camera> _camera)
{
	CameraContext = std::move(_camera);
}

void GameContext::SetWorld(World* _world)
{
	WorldContext = _world;
}

void GameContext::SetDeltaTime(float _dt)
{
	DeltaTimeValue = _dt;
}

ResourceHandler* GameContext::GetTextureHandler() const
{
	assert(TextureHandler);
	return TextureHandler.get();
}

Physics* GameContext::GetPhysics() const
{
	assert(PhysicsManager);
	return PhysicsManager.get();
}

Camera* GameContext::GetCamera() const
{
	assert(CameraContext);
	return CameraContext.get();
}

float GameContext::GetElapsedTime() const
{
	assert(WorldContext);
	return WorldContext->GetElapsedTime();
}

float GameContext::DeltaTime() const
{
	return DeltaTimeValue;
}

void GameContext::Instantiate(std::unique_ptr<Entity> _entity) const
{
	assert(WorldContext);
	WorldContext->AddObject(std::move(_entity));
}

TimerHandle GameContext::ScheduleTimer(float _delay, std::function<void()> _callback) const
{
	assert(WorldContext);
	return WorldContext->ScheduleTimer(_delay, _callback);
}

void GameContext::CancelTimer(TimerHandle _handle) const
{
	assert(WorldContext);
	WorldContext->CancelTimer(_handle);
}

void GameContext::Reset() const
{
	assert(WorldContext);
	WorldContext->Reset();
}

GameMode* GameContext::GetGameMode() const
{
	assert(WorldContext);
	return WorldContext->GetGameMode();
}
