#pragma once

#include <functional>
#include <memory>
#include <core/Timer.h>
#include <string>

class Entity;
class GameMode;
class GameStateEvents;
class InputManager;
class Physics;

class GameplayServices
{
public:
	virtual ~GameplayServices() = default;

	virtual float GetElapsedTime() const = 0;
	virtual float DeltaTime() const = 0;
	virtual Physics& GetPhysics() const = 0;
	virtual InputManager& GetInput() const = 0;
	virtual GameStateEvents& GetGameStateEvents() = 0;

	virtual std::unique_ptr<Entity> CreateEntity(const std::string& _texture, float _width, float _height) = 0;
	virtual void Instantiate(std::unique_ptr<Entity> _entity) const = 0;
	virtual TimerHandle ScheduleTimer(float _delay, std::function<void()> _callback) const = 0;
	virtual void CancelTimer(TimerHandle _handle) const = 0;
	virtual void Reset() const = 0;
	virtual GameMode* GetGameMode() const = 0;
};
