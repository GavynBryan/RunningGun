#pragma once
#include "ResourceHandler.h"
#include "Physics.h"
#include "World.h"
#include <functional>

class Environment
{
private:
	static Environment*									InstancePtr;

	std::unique_ptr<ResourceHandler>						TextureHandler;
	std::unique_ptr<Physics>								PhysicsManager;
	World*													WorldContext;
	float													DeltaTimeValue;

public:
	void													SetTextureHandler(std::unique_ptr<ResourceHandler> _handler);
	void													SetPhysics(std::unique_ptr<Physics> _physics);
	void													SetWorld(World* _world);
	void													SetDeltaTime(float _dt);

	ResourceHandler*										GetTextureHandler() const;
	Physics*												GetPhysics() const;
	float													GetElapsedTime() const;
	float													DeltaTime() const;

	void													Instantiate(std::unique_ptr<Entity> _entity) const;
	void													ScheduleTimer(float _delay, std::function<void()> _callback) const;

	void													Win() const;
	void													Reset() const;

public:
	Environment();
	~Environment();

	const static Environment& Instance();
};
