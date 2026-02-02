#pragma once

#include "FieldMeta.h"
#include <string_view>
#include <vector>
#include <memory>
#include <functional>

namespace simdjson::dom {
	class element;
}

namespace Json {
	class Writer;
}

//=============================================================================
// TypeMeta
//
// Generic metadata for any serializable type.
// Unlike ComponentMeta, this does not assume any base class or factory pattern.
// Can be used for plain structs, game objects, configuration, etc.
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

	// Serialize all fields to JSON
	void Serialize(const void* obj, Json::Writer& writer) const {
		for (const auto& field : Fields) {
			if (field.Serialize) {
				field.Serialize(obj, writer);
			}
		}
	}

	// Deserialize all fields from JSON
	void Deserialize(void* obj, const simdjson::dom::element& json) const {
		for (const auto& field : Fields) {
			if (field.Deserialize) {
				field.Deserialize(obj, json);
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
	static const TypeMeta& Get();
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

