#pragma once

#include <memory>

class Entity;
class EngineServices;
class PrefabComponentRegistry;
class PrefabDefinition;

class PrefabFactory
{
public:
	static std::unique_ptr<Entity> CreateEntity(EngineServices& _services, const PrefabDefinition& _definition, const PrefabComponentRegistry& _registry);
};
