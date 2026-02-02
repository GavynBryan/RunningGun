#pragma once

#include "TypeMeta.h"
#include "ISerializer.h"
#include "IDeserializer.h"
#include <core/services/framework/IService.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

//=============================================================================
// TypeRegistryService
//
// Service for registering and looking up serializable type metadata.
// Works with any class that uses SERIALIZABLE macros.
//
// Unlike singleton registries, this is a proper hosted service that must be
// explicitly added to the GameServiceHost and types must be explicitly
// registered.
//
// Usage:
//   // Setup
//   auto& registry = services.AddService<TypeRegistryService>();
//   registry.Register<PlayerConfig>();
//   registry.Register<EnemyConfig>();
//
//   // Lookup and use
//   auto& registry = services.Get<TypeRegistryService>();
//   const auto* meta = registry.Find("player_config");
//=============================================================================
class TypeRegistryService : public IService
{
public:
	TypeRegistryService() = default;

	// Register a type by its class (calls T::GetTypeMeta())
	template<typename T>
	void Register() {
		const auto& meta = T::GetTypeMeta();
		m_Registry[std::string(meta.TypeName)] = &meta;
		m_AllTypesDirty = true;
	}

	// Register a type with explicit metadata
	void Register(const TypeMeta& meta) {
		m_Registry[std::string(meta.TypeName)] = &meta;
		m_AllTypesDirty = true;
	}

	// Find metadata by type name (returns nullptr if not found)
	const TypeMeta* Find(std::string_view typeName) const {
		auto it = m_Registry.find(std::string(typeName));
		return it != m_Registry.end() ? it->second : nullptr;
	}

	// Check if a type is registered
	bool Contains(std::string_view typeName) const {
		return m_Registry.find(std::string(typeName)) != m_Registry.end();
	}

	// Get all registered types (for editor enumeration)
	const std::vector<const TypeMeta*>& All() const {
		if (m_AllTypesDirty) {
			m_AllTypes.clear();
			m_AllTypes.reserve(m_Registry.size());
			for (const auto& [name, meta] : m_Registry) {
				m_AllTypes.push_back(meta);
			}
			m_AllTypesDirty = false;
		}
		return m_AllTypes;
	}

	// Create an instance using the type's default factory
	// Returns nullptr if type not found or has no factory
	template<typename T = void>
	T* Create(std::string_view typeName) const {
		const auto* meta = Find(typeName);
		if (!meta || !meta->DefaultFactory) {
			return nullptr;
		}
		return static_cast<T*>(meta->DefaultFactory());
	}

	// Create and deserialize from any source
	template<typename T = void>
	T* CreateAndDeserialize(std::string_view typeName, const IDeserializer& deserializer) const {
		const auto* meta = Find(typeName);
		if (!meta || !meta->DefaultFactory) {
			return nullptr;
		}
		T* obj = static_cast<T*>(meta->DefaultFactory());
		meta->Deserialize(obj, deserializer);
		return obj;
	}

	// Serialize an object using its type name
	void Serialize(std::string_view typeName, const void* obj, ISerializer& serializer) const {
		const auto* meta = Find(typeName);
		if (meta) {
			meta->Serialize(obj, serializer);
		}
	}

	// Deserialize into an existing object
	void Deserialize(std::string_view typeName, void* obj, const IDeserializer& deserializer) const {
		const auto* meta = Find(typeName);
		if (meta) {
			meta->Deserialize(obj, deserializer);
		}
	}

	// Destroy an object created by the factory
	void Destroy(std::string_view typeName, void* obj) const {
		const auto* meta = Find(typeName);
		if (meta && meta->Destructor && obj) {
			meta->Destructor(obj);
		}
	}

private:
	std::unordered_map<std::string, const TypeMeta*> m_Registry;
	mutable std::vector<const TypeMeta*> m_AllTypes;
	mutable bool m_AllTypesDirty = true;
};

