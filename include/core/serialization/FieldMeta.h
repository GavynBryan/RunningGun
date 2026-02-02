#pragma once

#include <string_view>
#include <functional>
#include <cfloat>
#include <cstdint>
#include <type_traits>

// Forward declarations for abstract interfaces
class ISerializer;
class IDeserializer;

//=============================================================================
// FieldType
//
// Enum identifying the type of a serializable field.
// Used for editor UI type selection and validation.
//=============================================================================
enum class FieldType : uint8_t
{
	Float,
	Int,
	Bool,
	String,
	Vec2,
	Custom    // For nested/complex types
};

//=============================================================================
// FieldMeta
//
// Metadata for a single serializable field in any class.
// Uses abstract ISerializer/IDeserializer interfaces for format-agnostic
// serialization (JSON, binary, debug output, etc.)
//=============================================================================
struct FieldMeta
{
	std::string_view Name;           // Field name (used as key)
	std::string_view DisplayName;    // Human-readable name for editor
	std::string_view Tooltip;        // Editor tooltip/description
	FieldType Type = FieldType::Float;

	// Numeric constraints for editor sliders/validation
	float Min = -FLT_MAX;
	float Max = FLT_MAX;

	// Type-erased serialization callbacks using abstract interfaces
	// obj pointer is the address of the containing object
	std::function<void(void* obj, const IDeserializer& source)> Deserialize;
	std::function<void(const void* obj, ISerializer& dest)> Serialize;
};

//=============================================================================
// FieldTypeFor<T>()
//
// Maps C++ types to FieldType enum for automatic type detection.
// Add specializations for custom types.
//=============================================================================
template<typename T>
constexpr FieldType FieldTypeFor() {
	return FieldType::Custom;
}

template<> constexpr FieldType FieldTypeFor<float>()       { return FieldType::Float; }
template<> constexpr FieldType FieldTypeFor<double>()      { return FieldType::Float; }
template<> constexpr FieldType FieldTypeFor<int>()         { return FieldType::Int; }
template<> constexpr FieldType FieldTypeFor<int64_t>()     { return FieldType::Int; }
template<> constexpr FieldType FieldTypeFor<bool>()        { return FieldType::Bool; }
template<> constexpr FieldType FieldTypeFor<std::string>() { return FieldType::String; }

// Vec2 specialization - uses Vector2<float> template from core/math/Vec2.h
// Forward declare the template to avoid circular header dependency
template<typename T> struct Vector2;
using Vec2 = Vector2<float>;
template<> constexpr FieldType FieldTypeFor<Vec2>() { return FieldType::Vec2; }

