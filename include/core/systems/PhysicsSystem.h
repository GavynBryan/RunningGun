#pragma once

#include <core/Entity.h>
#include <vector>

class GameServiceHost;

class PhysicsSystem final
{
public:
	explicit PhysicsSystem(GameServiceHost& services);

	void Update();

private:
	GameServiceHost& Services;

	void UpdateEntityCollisions(const std::vector<Entity::Ptr>& entities);
};
