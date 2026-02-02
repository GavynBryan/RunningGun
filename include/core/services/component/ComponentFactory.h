#pragma once

#include <core/serialization/ComponentMeta.h>
#include <core/services/framework/IService.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <memory>

class ActorComponent;
class Actor;
class GameServiceHost;
class IDeserializer;

//=============================================================================
// ComponentFactoryService
//
// Service for registering and instantiating component types.
// Replaces the old singleton-based ComponentFactory.
//
// Components must be explicitly registered. The factory can be queried to:
//   - Instantiate components by type name
//   - Enumerate all registered components (for editor)
//   - Access property metadata (for serialization/editor)
//
// Usage:
//   auto& factory = services.AddService<ComponentFactoryService>();
//   factory.Register<PlayerComponent>();
//   factory.Register<RigidBody2DComponent>();
//
//   auto component = factory.Create("player", actor, services);
//=============================================================================
class ComponentFactoryService : public IService
{
public:
	ComponentFactoryService() = default;

	// Register a component type by its class (calls T::GetComponentMeta())
	template<typename T>
	void Register() {
		ComponentMeta meta = T::GetComponentMeta();
		m_Registry[std::string(meta.TypeName)] = std::move(meta);
		m_AllComponentsDirty = true;
	}

	// Register a component type with explicit metadata
	void Register(ComponentMeta meta) {
		std::string key(meta.TypeName);
		m_Registry[key] = std::move(meta);
		m_AllComponentsDirty = true;
	}

	// Find metadata by type name (returns nullptr if not found)
	const ComponentMeta* Find(std::string_view typeName) const {
		auto it = m_Registry.find(std::string(typeName));
		return (it != m_Registry.end()) ? &it->second : nullptr;
	}

	// Check if a type is registered
	bool Contains(std::string_view typeName) const {
		return m_Registry.find(std::string(typeName)) != m_Registry.end();
	}

	// Get all registered component types (for editor enumeration)
	const std::vector<const ComponentMeta*>& All() const {
		if (m_AllComponentsDirty) {
			m_AllComponents.clear();
			m_AllComponents.reserve(m_Registry.size());
			for (const auto& [name, meta] : m_Registry) {
				m_AllComponents.push_back(&meta);
			}
			m_AllComponentsDirty = false;
		}
		return m_AllComponents;
	}

	// Create a component instance by type name
	// Returns nullptr if type not found
	std::unique_ptr<ActorComponent> Create(
		std::string_view typeName,
		Actor& actor,
		GameServiceHost& services) const {
		const auto* meta = Find(typeName);
		if (!meta || !meta->Factory) {
			return nullptr;
		}
		return meta->Factory(actor, services);
	}

	// Create and deserialize a component
	std::unique_ptr<ActorComponent> CreateAndDeserialize(
		std::string_view typeName,
		Actor& actor,
		GameServiceHost& services,
		const IDeserializer& deserializer) const;

private:
	std::unordered_map<std::string, ComponentMeta> m_Registry;
	mutable std::vector<const ComponentMeta*> m_AllComponents;
	mutable bool m_AllComponentsDirty = true;
};

