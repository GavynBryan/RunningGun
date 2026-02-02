#pragma once

#include "FieldMeta.h"
#include <string_view>
#include <vector>
#include <memory>
#include <functional>

// Forward declarations for abstract interfaces
class ISerializer;
class IDeserializer;

//=============================================================================
// TypeMeta
//
// Generic metadata for any serializable type.
// Uses abstract ISerializer/IDeserializer interfaces for format-agnostic
// serialization (JSON, binary for networking, debug output, etc.)
//
// Unlike component-specific metadata, this does not assume any base class
// or factory pattern. Can be used for plain structs, game objects,
// configuration, network packets, etc.
//=============================================================================
struct TypeMeta
{
	std::string_view TypeName;              // Unique type identifier for serialization
	std::vector<FieldMeta> Fields;          // All serializable fields

	// Optional factory for creating default instances (type-erased)
	// Returns void* that caller must cast appropriately
	std::function<void*()> DefaultFactory;

	// Optional destructor for cleaning up factory-created instances
	std::function<void(void*)> Destructor;

	// Serialize all fields using the abstract interface
	void Serialize(const void* obj, ISerializer& serializer) const {
		for (const auto& field : Fields) {
			if (field.Serialize) {
				field.Serialize(obj, serializer);
			}
		}
	}

	// Deserialize all fields using the abstract interface
	void Deserialize(void* obj, const IDeserializer& deserializer) const {
		for (const auto& field : Fields) {
			if (field.Deserialize) {
				field.Deserialize(obj, deserializer);
			}
		}
	}
};

//=============================================================================
// TypeMetaOf<T>
//
// Type trait to access TypeMeta for a given type.
// Types using SERIALIZABLE macro will specialize this.
//=============================================================================
template<typename T>
struct TypeMetaOf {
	static const TypeMeta& Get() {
		return T::GetTypeMeta();
	}
};

//=============================================================================
// HasTypeMeta<T>
//
// SFINAE helper to check if a type has serialization metadata.
//=============================================================================
template<typename T, typename = void>
struct HasTypeMeta : std::false_type {};

template<typename T>
struct HasTypeMeta<T, std::void_t<decltype(T::GetTypeMeta())>> : std::true_type {};

template<typename T>
constexpr bool HasTypeMeta_v = HasTypeMeta<T>::value;

