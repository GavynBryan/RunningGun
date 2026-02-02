#pragma once

//=============================================================================
// Serializable.h
//
// Unified serialization macros for any class type, including components.
// Uses abstract ISerializer/IDeserializer interfaces for format-agnostic
// serialization (JSON, binary for networking, debug output, etc.)
//
// Basic Usage (for structs/classes):
//   struct PlayerConfig {
//       SERIALIZABLE_STRUCT(PlayerConfig, "player_config")
//
//       float Speed = 100.0f;
//       int MaxHealth = 100;
//   };
//
//   SERIALIZE_FIELDS(PlayerConfig,
//       FIELD(Speed, "Movement Speed", 100.0f, .Min = 0.0f, .Max = 1000.0f)
//       FIELD(MaxHealth, "Max Health", 100)
//   )
//
// Component Usage:
//   class PlayerComponent : public ActorComponent {
//       SERIALIZABLE_COMPONENT(PlayerComponent, "player")
//   private:
//       float m_Speed;
//       int m_Lives;
//   };
//
//   SERIALIZE_COMPONENT(PlayerComponent,
//       PROPERTY(float, Speed, 350.0f, .DisplayName = "Player Speed")
//       PROPERTY(int, Lives, 5, .DisplayName = "Starting Lives")
//   )
//
// To serialize/deserialize:
//   PlayerConfig config;
//   JsonSerializer serializer;
//   serializer.BeginObject();
//   config.Serialize(serializer);
//   serializer.EndObject();
//
//   auto deserializer = JsonDeserializer::Parse(jsonString);
//   config.Deserialize(*deserializer);
//=============================================================================

#include "TypeMeta.h"
#include "FieldMeta.h"
#include "ISerializer.h"
#include "IDeserializer.h"

// Forward declarations
class Actor;
class GameServiceHost;
class ActorComponent;

//=============================================================================
// SERIALIZABLE(ClassName, TypeName)
//
// Marks a class as serializable. Place at the start of the class body.
// For structs, use SERIALIZABLE_STRUCT instead.
//
// - ClassName: The C++ class name
// - TypeName: String identifier for serialization
//=============================================================================
#define SERIALIZABLE(ClassName, TypeName)                                       \
public:                                                                         \
	static constexpr std::string_view StaticTypeName() { return TypeName; }     \
	static const TypeMeta& GetTypeMeta();                                       \
	void Serialize(ISerializer& serializer) const {                             \
		GetTypeMeta().Serialize(this, serializer);                              \
	}                                                                           \
	void Deserialize(const IDeserializer& deserializer) {                       \
		GetTypeMeta().Deserialize(this, deserializer);                          \
	}                                                                           \
private:

//=============================================================================
// SERIALIZABLE_STRUCT(ClassName, TypeName)
//
// Same as SERIALIZABLE but for structs (doesn't affect visibility).
//=============================================================================
#define SERIALIZABLE_STRUCT(ClassName, TypeName)                                \
	static constexpr std::string_view StaticTypeName() { return TypeName; }     \
	static const TypeMeta& GetTypeMeta();                                       \
	void Serialize(ISerializer& serializer) const {                             \
		GetTypeMeta().Serialize(this, serializer);                              \
	}                                                                           \
	void Deserialize(const IDeserializer& deserializer) {                       \
		GetTypeMeta().Deserialize(this, deserializer);                          \
	}

//=============================================================================
// SERIALIZE_FIELDS(ClassName, ...)
//
// Defines the serializable fields for a class.
// Place in a .cpp file or after the class definition.
//=============================================================================
#define SERIALIZE_FIELDS(ClassName, ...)                                        \
	const TypeMeta& ClassName::GetTypeMeta() {                                  \
		using SelfType = ClassName;                                             \
		static TypeMeta meta = [] {                                             \
			TypeMeta m;                                                         \
			m.TypeName = ClassName::StaticTypeName();                           \
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
			m.TypeName = ClassName::StaticTypeName();                           \
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
// same name. Automatically detects the type.
//
// - Name: Member variable name (used as key)
// - DisplayName: Human-readable name for editor UI
// - Default: Default value if not present in data
// - ...: Optional metadata (.Tooltip, .Min, .Max)
//=============================================================================
#define FIELD(Name, DisplayName, Default, ...)                                  \
	FieldMeta{                                                                  \
		.Name = #Name,                                                          \
		.DisplayName = DisplayName,                                             \
		.Tooltip = "",                                                          \
		.Type = FieldTypeFor<decltype(SelfType::Name)>(),                       \
		__VA_ARGS__                                                             \
		.Deserialize = [](void* obj, const IDeserializer& src) {                \
			auto* self = static_cast<SelfType*>(obj);                           \
			self->Name = src.Get<std::decay_t<decltype(self->Name)>>(           \
				#Name, Default);                                                \
		},                                                                      \
		.Serialize = [](const void* obj, ISerializer& dest) {                   \
			const auto* self = static_cast<const SelfType*>(obj);               \
			dest.Write(#Name, self->Name);                                      \
		}                                                                       \
	},

//=============================================================================
// FIELD_CUSTOM(Name, DisplayName, DeserializeFn, SerializeFn, ...)
//
// Define a field with custom serialization logic.
//
// DeserializeFn signature: void(void* obj, const IDeserializer& src)
// SerializeFn signature: void(const void* obj, ISerializer& dest)
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
		.Deserialize = [](void* obj, const IDeserializer& src) {                \
			auto* self = static_cast<SelfType*>(obj);                           \
			auto nested = src.GetObject(#Name);                                 \
			if (nested) {                                                       \
				self->Name.Deserialize(*nested);                                \
			}                                                                   \
		},                                                                      \
		.Serialize = [](const void* obj, ISerializer& dest) {                   \
			const auto* self = static_cast<const SelfType*>(obj);               \
			dest.BeginObject(#Name);                                            \
			self->Name.Serialize(dest);                                         \
			dest.EndObject();                                                   \
		}                                                                       \
	},

//=============================================================================
//
// COMPONENT SERIALIZATION MACROS
//
// These macros are specifically for ActorComponent-derived classes.
// They integrate with the ComponentFactory and use the m_ prefix convention.
//
//=============================================================================

#include "ComponentMeta.h"

//=============================================================================
// SERIALIZABLE_COMPONENT(ClassName, TypeName)
//
// Marks an ActorComponent-derived class as serializable.
// Place at the start of the class body (after opening brace).
//
// - ClassName: The C++ class name
// - TypeName: String used for component lookup (e.g., "player", "rigidbody2d")
//=============================================================================
#define SERIALIZABLE_COMPONENT(ClassName, TypeName)                             \
public:                                                                         \
	static constexpr std::string_view StaticTypeName() { return TypeName; }     \
	static ComponentMeta GetComponentMeta();                                    \
	void Serialize(ISerializer& serializer) const override;                     \
	void Deserialize(const IDeserializer& deserializer) override;               \
	const char* GetTypeName() const override { return TypeName; }               \
private:

//=============================================================================
// SERIALIZE_COMPONENT(ClassName, ...)
//
// Defines the serializable properties for a component.
// Uses the m_ prefix convention (PROPERTY(float, Speed, ...) maps to m_Speed).
// Place in the .cpp file.
//=============================================================================
#define SERIALIZE_COMPONENT(ClassName, ...)                                     \
	ComponentMeta ClassName::GetComponentMeta() {                               \
		using SelfType = ClassName;                                             \
		ComponentMeta meta;                                                     \
		meta.TypeName = StaticTypeName();                                       \
		meta.Factory = [](Actor& actor, GameServiceHost& services) {            \
			return std::make_unique<ClassName>(actor, services);                \
		};                                                                      \
		meta.Properties = { __VA_ARGS__ };                                      \
		return meta;                                                            \
	}                                                                           \
	void ClassName::Serialize(ISerializer& serializer) const {                  \
		const auto& meta = GetComponentMeta();                                  \
		for (const auto& prop : meta.Properties) {                              \
			if (prop.Serialize) {                                               \
				prop.Serialize(this, serializer);                               \
			}                                                                   \
		}                                                                       \
	}                                                                           \
	void ClassName::Deserialize(const IDeserializer& deserializer) {            \
		const auto& meta = GetComponentMeta();                                  \
		for (const auto& prop : meta.Properties) {                              \
			if (prop.Deserialize) {                                             \
				prop.Deserialize(this, deserializer);                           \
			}                                                                   \
		}                                                                       \
	}

//=============================================================================
// NO_COMPONENT_PROPERTIES(ClassName)
//
// Use for components with no serializable properties.
//=============================================================================
#define NO_COMPONENT_PROPERTIES(ClassName)                                      \
	ComponentMeta ClassName::GetComponentMeta() {                               \
		ComponentMeta meta;                                                     \
		meta.TypeName = StaticTypeName();                                       \
		meta.Factory = [](Actor& actor, GameServiceHost& services) {            \
			return std::make_unique<ClassName>(actor, services);                \
		};                                                                      \
		return meta;                                                            \
	}                                                                           \
	void ClassName::Serialize(ISerializer&) const {}                            \
	void ClassName::Deserialize(const IDeserializer&) {}

//=============================================================================
// PROPERTY(Type, Name, Default, ...)
//
// Declares a serializable component property with the m_ prefix convention.
// - Type: C++ type (float, int, bool, std::string, Vec2)
// - Name: Property name (member variable will be m_Name)
// - Default: Default value if not present in data
// - ...: Optional metadata (.DisplayName, .Tooltip, .Min, .Max)
//=============================================================================
#define PROPERTY(Type, Name, Default, ...)                                      \
	FieldMeta{                                                                  \
		.Name = #Name,                                                          \
		.DisplayName = #Name,                                                   \
		.Tooltip = "",                                                          \
		.Type = FieldTypeFor<Type>(),                                           \
		__VA_ARGS__                                                             \
		.Deserialize = [](void* obj, const IDeserializer& src) {                \
			auto* self = static_cast<SelfType*>(obj);                           \
			self->m_##Name = src.Get<Type>(#Name, Default);                     \
		},                                                                      \
		.Serialize = [](const void* obj, ISerializer& dest) {                   \
			const auto* self = static_cast<const SelfType*>(obj);               \
			dest.Write(#Name, self->m_##Name);                                  \
		}                                                                       \
	},

