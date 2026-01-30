#include <core/engine/WorldServices.h>
#include <core/World.h>
#include <cassert>

void WorldServices::SetWorld(World* _world)
{
	WorldContext = _world;
}

float WorldServices::GetElapsedTime() const
{
	assert(WorldContext);
	return WorldContext->GetElapsedTime();
}

void WorldServices::Instantiate(std::unique_ptr<Entity> _entity) const
{
	assert(WorldContext);
	WorldContext->AddObject(std::move(_entity));
}

TimerHandle WorldServices::ScheduleTimer(float _delay, std::function<void()> _callback) const
{
	assert(WorldContext);
	return WorldContext->ScheduleTimer(_delay, _callback);
}

void WorldServices::CancelTimer(TimerHandle _handle) const
{
	assert(WorldContext);
	WorldContext->CancelTimer(_handle);
}

void WorldServices::Reset() const
{
	assert(WorldContext);
	WorldContext->Reset();
}

GameMode* WorldServices::GetGameMode() const
{
	assert(WorldContext);
	return WorldContext->GetGameMode();
}
