#include <core/Environment.h>

Environment *Environment::InstancePtr = nullptr;

Environment::Environment()
	: WorldContext(nullptr)
	, DeltaTimeValue(0)
{
	InstancePtr = this;
}


Environment::~Environment()
{
}

void Environment::SetTextureHandler(std::unique_ptr<ResourceHandler> _handler)
{
	TextureHandler = std::move(_handler);
}

void Environment::SetPhysics(std::unique_ptr<Physics> _physics)
{
	PhysicsManager = std::move(_physics);
}

void Environment::SetWorld(World* _world)
{
	WorldContext = _world;
}

void Environment::SetDeltaTime(float _dt)
{
	DeltaTimeValue = _dt;
}

ResourceHandler* Environment::GetTextureHandler() const
{
	assert(TextureHandler);
	return TextureHandler.get();
}

Physics* Environment::GetPhysics() const
{
	assert(PhysicsManager);
	return PhysicsManager.get();
}

float Environment::GetElapsedTime() const
{
	assert(WorldContext);
	return WorldContext->GetElapsedTime();
}

float Environment::DeltaTime() const
{
	return DeltaTimeValue;
}

void Environment::Instantiate(std::unique_ptr<Entity> _entity) const
{
	assert(WorldContext);
	WorldContext->AddObject(std::move(_entity));
}

void Environment::ScheduleTimer(float _delay, std::function<void()> _callback) const
{
	assert(WorldContext);
	WorldContext->ScheduleTimer(_delay, _callback);
}

const Environment& Environment::Instance()
{
	assert(InstancePtr);
	return *InstancePtr;
}

void Environment::Win() const
{
	assert(WorldContext);
	WorldContext->WinGame();
}

void Environment::Reset() const
{
	assert(WorldContext);
	WorldContext->Reset();
}
