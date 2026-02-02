#pragma once

#include "TypeMeta.h"
#include "Serializable.h"
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

//=============================================================================
// TypeRegistry
//
// Generic singleton registry for serializable type metadata.
// Works with any class that uses SERIALIZABLE macros.
// Unlike ComponentFactory, this doesn't assume any base class.
//
// Types self-register via TypeAutoRegister<T> at static init time.
// The registry can be queried to:
//   - Look up type metadata by name
//   - Enumerate all registered types
//   - Create instances via default factory
//   - Serialize/deserialize objects by type name
//=============================================================================
class TypeRegistry
{
public:
	// Get the singleton instance (lazy-initialized)
	static TypeRegistry& Instance() {
		static TypeRegistry instance;
		return instance;
	}

	// Non-copyable, non-movable
	TypeRegistry(const TypeRegistry&) = delete;
	TypeRegistry& operator=(const TypeRegistry&) = delete;

	// Register a type with its metadata
	void Register(const TypeMeta& meta) {
		auto name = std::string(meta.TypeName);
		m_Registry[name] = &meta;
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

	// Create and deserialize from JSON
	template<typename T = void>
	T* CreateFromJson(std::string_view typeName, const simdjson::dom::element& json) const {
		const auto* meta = Find(typeName);
		if (!meta || !meta->DefaultFactory) {
			return nullptr;
		}
		T* obj = static_cast<T*>(meta->DefaultFactory());
		meta->Deserialize(obj, json);
		return obj;
	}

	// Serialize an object to JSON
	void Serialize(std::string_view typeName, const void* obj, Json::Writer& writer) const {
		const auto* meta = Find(typeName);
		if (meta) {
			meta->Serialize(obj, writer);
		}
	}

	// Deserialize JSON into an existing object
	void Deserialize(std::string_view typeName, void* obj, const simdjson::dom::element& json) const {
		const auto* meta = Find(typeName);
		if (meta) {
			meta->Deserialize(obj, json);
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
	TypeRegistry() = default;

	std::unordered_map<std::string, const TypeMeta*> m_Registry;
	mutable std::vector<const TypeMeta*> m_AllTypes;
	mutable bool m_AllTypesDirty = true;
};

//=============================================================================
// TypeAutoRegister<T> implementation
//
// Registers the type with TypeRegistry at static initialization.
//=============================================================================
template<typename T>
TypeAutoRegister<T>::TypeAutoRegister()
{
	TypeRegistry::Instance().Register(T::GetTypeMeta());
}

