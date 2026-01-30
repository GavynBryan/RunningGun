#pragma once

#include <functional>
#include <memory>
#include <core/Timer.h>

class Entity;
class GameMode;
class World;
class EngineServices;

class WorldServices
{
public:
	float GetElapsedTime() const;
	void Instantiate(std::unique_ptr<Entity> _entity) const;
	TimerHandle ScheduleTimer(float _delay, std::function<void()> _callback) const;
	void CancelTimer(TimerHandle _handle) const;
	void Reset() const;
	GameMode* GetGameMode() const;

private:
	World* WorldContext = nullptr;

	void SetWorld(World* _world);

	friend class EngineServices;
};
