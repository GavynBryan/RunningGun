#pragma once

#include <core/serialization/ComponentMeta.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <memory>

class ActorComponent;
class Actor;
class GameServiceHost;

//=============================================================================
// ComponentRegistry
// 
// Singleton registry for component type metadata and factories.
// Uses lazy initialization to avoid static initialization order issues.
// 
// Components self-register via ComponentAutoRegister<T> at static init time.
// The registry can be queried to:
//   - Instantiate components by type name
//   - Enumerate all registered components (for editor)
//   - Access property metadata (for serialization/editor)
//=============================================================================
class ComponentRegistry
{
public:
	// Get the singleton instance (lazy-initialized)
	static ComponentRegistry& Instance();

	// Non-copyable, non-movable
	ComponentRegistry(const ComponentRegistry&) = delete;
	ComponentRegistry& operator=(const ComponentRegistry&) = delete;

	// Register a component type with its metadata
	void Register(ComponentMeta meta);

	// Find metadata by type name (returns nullptr if not found)
	const ComponentMeta* Find(std::string_view typeName) const;

	// Check if a type is registered
	bool Contains(std::string_view typeName) const;

	// Get all registered component types (for editor enumeration)
	const std::vector<const ComponentMeta*>& All() const { return m_AllComponents; }

	// Create a component instance by type name
	// Returns nullptr if type not found
	std::unique_ptr<ActorComponent> Create(
		std::string_view typeName,
		Actor& actor,
		GameServiceHost& services) const;

	// Create and deserialize a component from JSON
	std::unique_ptr<ActorComponent> CreateFromJson(
		std::string_view typeName,
		Actor& actor,
		GameServiceHost& services,
		const simdjson::dom::element& json) const;

private:
	ComponentRegistry() = default;

	std::unordered_map<std::string, ComponentMeta> m_Registry;
	mutable std::vector<const ComponentMeta*> m_AllComponents;
	mutable bool m_AllComponentsDirty = true;
};

//=============================================================================
// ComponentAutoRegister<T>
// 
// Template that auto-registers a component at static initialization.
// Each component class using COMPONENT() macro gets one of these.
//=============================================================================
template<typename T>
ComponentAutoRegister<T>::ComponentAutoRegister()
{
	ComponentRegistry::Instance().Register(T::GetComponentMeta());
}
