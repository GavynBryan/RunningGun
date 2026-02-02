#pragma once

//=============================================================================
// ComponentMacros.h
// 
// Provides macros for self-registering components with automatic serialization.
// Components use these macros to declare serializable properties with metadata
// for editor UI (display names, tooltips, min/max constraints).
//
// Usage:
//   // In header (.h):
//   class MyComponent : public ActorComponent {
//       COMPONENT(MyComponent, "mycomponent")
//   public:
//       MyComponent(Actor& actor, GameServiceHost& services);
//       // ... methods ...
//   private:
//       float m_Speed;      // Will be serialized
//       int m_Lives;        // Will be serialized
//       float m_Timer = 0;  // Runtime state, not serialized
//   };
//
//   // In source (.cpp):
//   BEGIN_PROPERTIES(MyComponent)
//       PROPERTY(float, Speed, 100.0f,
//           .DisplayName = "Movement Speed",
//           .Tooltip = "Units per second",
//           .Min = 0.0f, .Max = 1000.0f)
//       PROPERTY(int, Lives, 3,
//           .DisplayName = "Starting Lives",
//           .Min = 1, .Max = 99)
//   END_PROPERTIES()
//
//   MyComponent::MyComponent(Actor& actor, GameServiceHost& services)
//       : ActorComponent(actor, services) {
//       // m_Speed and m_Lives initialized by DeserializeProperties()
//   }
//=============================================================================

#include <core/serialization/PropertyMeta.h>
#include <core/serialization/ComponentMeta.h>
#include <core/util/Json.h>

// Forward declare the factory for auto-registration
class ComponentFactory;

//-----------------------------------------------------------------------------
// Auto-registration helper template
// Creates a static instance that registers the component at program startup
//-----------------------------------------------------------------------------
template<typename T>
struct ComponentAutoRegister
{
	ComponentAutoRegister();  // Defined after ComponentFactory is available
};

//-----------------------------------------------------------------------------
// COMPONENT(ClassName, TypeName)
// 
// Declares a component as serializable with auto-registration.
// Place at the start of the class body (after opening brace).
// 
// - ClassName: The C++ class name
// - TypeName: String used in JSON (e.g., "player", "rigidbody2d")
//-----------------------------------------------------------------------------
#define COMPONENT(ClassName, TypeName)                                          \
	private:                                                                    \
		inline static ComponentAutoRegister<ClassName> s_AutoRegister{};        \
	public:                                                                     \
		static constexpr std::string_view StaticTypeName() { return TypeName; } \
		static ComponentMeta GetComponentMeta();                                \
		void SerializeProperties(Json::Writer& writer) const override;          \
		void DeserializeProperties(const simdjson::dom::element& json) override;\
		const char* GetTypeName() const override { return TypeName; }           \
	private:

//-----------------------------------------------------------------------------
// BEGIN_PROPERTIES(ClassName)
// 
// Begins the property definition block in a .cpp file.
// Must be followed by PROPERTY() entries and END_PROPERTIES().
//-----------------------------------------------------------------------------
#define BEGIN_PROPERTIES(ClassName)                                             \
	ComponentMeta ClassName::GetComponentMeta() {                               \
		using SelfType = ClassName;                                             \
		ComponentMeta meta;                                                     \
		meta.TypeName = StaticTypeName();                                       \
		meta.Factory = [](Actor& actor, GameServiceHost& services) {            \
			return std::make_unique<ClassName>(actor, services);                \
		};                                                                      \
		meta.Properties = {

//-----------------------------------------------------------------------------
// PROPERTY(Type, Name, Default, ...)
// 
// Declares a serializable property with metadata.
// - Type: C++ type (float, int, bool, std::string, Vec2)
// - Name: Property name (member will be m_Name)
// - Default: Default value if not present in JSON
// - ...: Optional metadata (.DisplayName, .Tooltip, .Min, .Max)
//
// The property must have a corresponding member variable named m_Name.
//-----------------------------------------------------------------------------
#define PROPERTY(Type, Name, Default, ...)                                      \
		PropertyMeta{                                                           \
			.Name = #Name,                                                      \
			.DisplayName = #Name,                                               \
			.Tooltip = "",                                                      \
			.Type = PropertyTypeFor<Type>(),                                    \
			__VA_ARGS__                                                         \
			.Deserialize = [](void* obj, const simdjson::dom::element& json) {  \
				auto* self = static_cast<SelfType*>(obj);                       \
				self->m_##Name = Json::Get<Type>(json, #Name, Default);         \
			},                                                                  \
			.Serialize = [](const void* obj, Json::Writer& writer) {            \
				const auto* self = static_cast<const SelfType*>(obj);           \
				writer.Write(#Name, self->m_##Name);                            \
			}                                                                   \
		},

//-----------------------------------------------------------------------------
// END_PROPERTIES()
// 
// Ends the property definition block and generates serialization methods.
//-----------------------------------------------------------------------------
#define END_PROPERTIES(ClassName)                                               \
		};                                                                      \
		return meta;                                                            \
	}                                                                           \
	void ClassName::SerializeProperties(Json::Writer& writer) const {           \
		const auto& meta = GetComponentMeta();                                  \
		for (const auto& prop : meta.Properties) {                              \
			prop.Serialize(this, writer);                                       \
		}                                                                       \
	}                                                                           \
	void ClassName::DeserializeProperties(const simdjson::dom::element& json) { \
		const auto& meta = GetComponentMeta();                                  \
		for (const auto& prop : meta.Properties) {                              \
			prop.Deserialize(this, json);                                       \
		}                                                                       \
	}

//-----------------------------------------------------------------------------
// NO_PROPERTIES(ClassName)
// 
// Use for components with no serializable properties.
// Still enables auto-registration and type identification.
//-----------------------------------------------------------------------------
#define NO_PROPERTIES(ClassName)                                                \
	ComponentMeta ClassName::GetComponentMeta() {                               \
		ComponentMeta meta;                                                     \
		meta.TypeName = StaticTypeName();                                       \
		meta.Factory = [](Actor& actor, GameServiceHost& services) {            \
			return std::make_unique<ClassName>(actor, services);                \
		};                                                                      \
		return meta;                                                            \
	}                                                                           \
	void ClassName::SerializeProperties(Json::Writer&) const {}                 \
	void ClassName::DeserializeProperties(const simdjson::dom::element&) {}

//-----------------------------------------------------------------------------
// PropertyTypeFor<T>()
// 
// Maps C++ types to PropertyType enum for editor type identification.
//-----------------------------------------------------------------------------
template<typename T> constexpr PropertyType PropertyTypeFor();
template<> constexpr PropertyType PropertyTypeFor<float>()       { return PropertyType::Float; }
template<> constexpr PropertyType PropertyTypeFor<int>()         { return PropertyType::Int; }
template<> constexpr PropertyType PropertyTypeFor<bool>()        { return PropertyType::Bool; }
template<> constexpr PropertyType PropertyTypeFor<std::string>() { return PropertyType::String; }
template<> constexpr PropertyType PropertyTypeFor<Vec2>()        { return PropertyType::Vec2; }
