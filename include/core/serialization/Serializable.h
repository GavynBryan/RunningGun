#pragma once

//=============================================================================
// Serializable.h
//
// Generic serialization macros that work with any class type.
// Provides a seamless developer experience with minimal boilerplate.
//
// Basic Usage:
//   struct PlayerConfig {
//       SERIALIZABLE_STRUCT(PlayerConfig, "player_config")
//
//       float Speed = 100.0f;
//       int MaxHealth = 100;
//       std::string Name = "Player";
//   };
//
//   SERIALIZE_FIELDS(PlayerConfig,
//       FIELD(Speed, "Movement Speed", 100.0f, .Min = 0.0f, .Max = 1000.0f)
//       FIELD(MaxHealth, "Max Health", 100, .Min = 1, .Max = 999)
//       FIELD(Name, "Player Name", "Player")
//   )
//
// Advanced Usage with custom types:
//   SERIALIZE_FIELDS(MyClass,
//       FIELD_CUSTOM(Data, "Data",
//           [](void* obj, const auto& json) { /* deserialize */ },
//           [](const void* obj, auto& writer) { /* serialize */ })
//   )
//
// For nested serializable types:
//   SERIALIZE_FIELDS(MyClass,
//       FIELD_NESTED(Config, "Configuration")
//   )
//
// To serialize/deserialize:
//   PlayerConfig config;
//   auto& meta = PlayerConfig::GetTypeMeta();
//
//   // Serialize to JSON
//   Json::Writer writer;
//   writer.BeginObject();
//   meta.Serialize(&config, writer);
//   writer.EndObject();
//
//   // Deserialize from JSON
//   auto json = Json::Parse(jsonString);
//   meta.Deserialize(&config, json.value());
//=============================================================================

#include "TypeMeta.h"
#include "FieldMeta.h"
#include <core/util/Json.h>

//=============================================================================
// Forward declaration for TypeRegistry
//=============================================================================
class TypeRegistry;

//=============================================================================
// Auto-registration helper for generic types
//=============================================================================
template<typename T>
struct TypeAutoRegister
{
	TypeAutoRegister();
};

//=============================================================================
// SERIALIZABLE(ClassName, TypeName)
//
// Marks a class as serializable and enables auto-registration.
// Place at the start of the class body.
//
// - ClassName: The C++ class name
// - TypeName: String identifier for JSON serialization
//=============================================================================
#define SERIALIZABLE(ClassName, TypeName)                                       \
private:                                                                        \
	inline static TypeAutoRegister<ClassName> s_TypeAutoRegister{};             \
public:                                                                         \
	static constexpr std::string_view StaticTypeName() { return TypeName; }     \
	static const TypeMeta& GetTypeMeta();                                       \
	void SerializeFields(Json::Writer& writer) const {                          \
		GetTypeMeta().Serialize(this, writer);                                  \
	}                                                                           \
	void DeserializeFields(const simdjson::dom::element& json) {                \
		GetTypeMeta().Deserialize(this, json);                                  \
	}                                                                           \
private:

//=============================================================================
// SERIALIZABLE_STRUCT(ClassName, TypeName)
//
// Same as SERIALIZABLE but for structs (doesn't affect visibility).
//=============================================================================
#define SERIALIZABLE_STRUCT(ClassName, TypeName)                                \
	inline static TypeAutoRegister<ClassName> s_TypeAutoRegister{};             \
	static constexpr std::string_view StaticTypeName() { return TypeName; }     \
	static const TypeMeta& GetTypeMeta();                                       \
	void SerializeFields(Json::Writer& writer) const {                          \
		GetTypeMeta().Serialize(this, writer);                                  \
	}                                                                           \
	void DeserializeFields(const simdjson::dom::element& json) {                \
		GetTypeMeta().Deserialize(this, json);                                  \
	}

//=============================================================================
// SERIALIZE_FIELDS(ClassName, ...)
//
// Defines the serializable fields for a class.
// Place in a .cpp file or after the class definition.
//
// Usage:
//   SERIALIZE_FIELDS(MyClass,
//       FIELD(Speed, "Movement Speed", .Min = 0.0f)
//       FIELD(Health, "Health Points")
//   )
//=============================================================================
#define SERIALIZE_FIELDS(ClassName, ...)                                        \
	const TypeMeta& ClassName::GetTypeMeta() {                                  \
		using SelfType = ClassName;                                             \
		static TypeMeta meta = [] {                                             \
			TypeMeta m;                                                         \
			m.TypeName = StaticTypeName();                                      \
			m.DefaultFactory = []() -> void* { return new ClassName(); };       \
			m.Destructor = [](void* p) { delete static_cast<ClassName*>(p); };  \
			m.Fields = { __VA_ARGS__ };                                         \
			return m;                                                           \
		}();                                                                    \
		return meta;                                                            \
	}

//=============================================================================
// SERIALIZE_FIELDS_NO_FACTORY(ClassName, ...)
//
// Same as SERIALIZE_FIELDS but without a default factory.
// Use when the class doesn't have a default constructor.
//=============================================================================
#define SERIALIZE_FIELDS_NO_FACTORY(ClassName, ...)                             \
	const TypeMeta& ClassName::GetTypeMeta() {                                  \
		using SelfType = ClassName;                                             \
		static TypeMeta meta = [] {                                             \
			TypeMeta m;                                                         \
			m.TypeName = StaticTypeName();                                      \
			m.Fields = { __VA_ARGS__ };                                         \
			return m;                                                           \
		}();                                                                    \
		return meta;                                                            \
	}

//=============================================================================
// NO_SERIALIZABLE_FIELDS(ClassName)
//
// For types that are serializable but have no fields.
//=============================================================================
#define NO_SERIALIZABLE_FIELDS(ClassName)                                       \
	const TypeMeta& ClassName::GetTypeMeta() {                                  \
		static TypeMeta meta = [] {                                             \
			TypeMeta m;                                                         \
			m.TypeName = ClassName::StaticTypeName();                           \
			m.DefaultFactory = []() -> void* { return new ClassName(); };       \
			m.Destructor = [](void* p) { delete static_cast<ClassName*>(p); };  \
			return m;                                                           \
		}();                                                                    \
		return meta;                                                            \
	}

//=============================================================================
// FIELD(Name, DisplayName, Default, ...)
//
// Defines a serializable field. The member variable must exist with the
// same name. Automatically detects the type and creates serialization
// callbacks.
//
// - Name: Member variable name (used as JSON key)
// - DisplayName: Human-readable name for editor UI
// - Default: Default value if not present in JSON
// - ...: Optional metadata (.Tooltip, .Min, .Max)
//
// Example:
//   FIELD(Speed, "Movement Speed", 100.0f, .Min = 0.0f, .Max = 1000.0f)
//=============================================================================
#define FIELD(Name, DisplayName, Default, ...)                                  \
	FieldMeta{                                                                  \
		.Name = #Name,                                                          \
		.DisplayName = DisplayName,                                             \
		.Tooltip = "",                                                          \
		.Type = FieldTypeFor<decltype(SelfType::Name)>(),                       \
		__VA_ARGS__                                                             \
		.Deserialize = [](void* obj, const simdjson::dom::element& json) {      \
			auto* self = static_cast<SelfType*>(obj);                           \
			self->Name = Json::Get<std::decay_t<decltype(self->Name)>>(         \
				json, #Name, Default);                                          \
		},                                                                      \
		.Serialize = [](const void* obj, Json::Writer& writer) {                \
			const auto* self = static_cast<const SelfType*>(obj);               \
			writer.Write(#Name, self->Name);                                    \
		}                                                                       \
	},

//=============================================================================
// FIELD_CUSTOM(Name, DisplayName, DeserializeFn, SerializeFn, ...)
//
// Define a field with custom serialization logic.
// Use for nested types or complex serialization requirements.
//
// DeserializeFn signature: void(void* obj, const simdjson::dom::element& json)
// SerializeFn signature: void(const void* obj, Json::Writer& writer)
//=============================================================================
#define FIELD_CUSTOM(Name, DisplayName, DeserializeFn, SerializeFn, ...)        \
	FieldMeta{                                                                  \
		.Name = #Name,                                                          \
		.DisplayName = DisplayName,                                             \
		.Tooltip = "",                                                          \
		.Type = FieldType::Custom,                                              \
		__VA_ARGS__                                                             \
		.Deserialize = DeserializeFn,                                           \
		.Serialize = SerializeFn                                                \
	},

//=============================================================================
// FIELD_NESTED(Name, DisplayName, ...)
//
// Serialize a nested serializable type. The nested type must also use
// SERIALIZABLE macros.
//=============================================================================
#define FIELD_NESTED(Name, DisplayName, ...)                                    \
	FieldMeta{                                                                  \
		.Name = #Name,                                                          \
		.DisplayName = DisplayName,                                             \
		.Tooltip = "",                                                          \
		.Type = FieldType::Custom,                                              \
		__VA_ARGS__                                                             \
		.Deserialize = [](void* obj, const simdjson::dom::element& json) {      \
			auto* self = static_cast<SelfType*>(obj);                           \
			auto nested = json[#Name];                                          \
			if (!nested.error()) {                                              \
				self->Name.DeserializeFields(nested.value());                   \
			}                                                                   \
		},                                                                       \
		.Serialize = [](const void* obj, Json::Writer& writer) {                \
			const auto* self = static_cast<const SelfType*>(obj);               \
			writer.BeginObject(#Name);                                          \
			self->Name.SerializeFields(writer);                                 \
			writer.EndObject();                                                 \
		}                                                                       \
	},

