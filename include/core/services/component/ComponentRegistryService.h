#pragma once

#include <core/framework/IService.h>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

class ActorComponent;
class Actor;
class GameServiceHost;

// Service that manages component factory registration.
// Components can be registered by type name and instantiated dynamically
// (e.g., when loading prefabs from JSON).
class ComponentRegistryService final : public IService
{
public:
	// Factory takes Entity (for owner assignment) and services, plus optional JSON params
	using Factory = std::function<std::unique_ptr<ActorComponent>(Actor&, GameServiceHost&, std::string_view)>;

	ComponentRegistryService() = default;
	~ComponentRegistryService() override = default;

	// Register a component factory by type name
	void Register(std::string_view type, Factory factory);
	
	// Find a factory by type name (returns nullptr if not found)
	const Factory* Find(std::string_view type) const;
	
	// Check if a type is registered
	bool Contains(std::string_view type) const;
	
	// Get the number of registered component types
	size_t Count() const;

private:
	std::unordered_map<std::string, Factory> Factories;
};
