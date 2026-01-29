#pragma once

#include <memory>

class Entity;
class GameContext;
class PrefabComponentRegistry;
class PrefabDefinition;

class PrefabFactory
{
public:
	static std::unique_ptr<Entity> CreateEntity(GameContext& _context, const PrefabDefinition& _definition, const PrefabComponentRegistry& _registry);
};
