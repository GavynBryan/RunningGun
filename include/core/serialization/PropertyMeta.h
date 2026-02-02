#pragma once

#include <string_view>
#include <functional>
#include <cfloat>
#include <cstdint>

// Forward declarations
namespace simdjson::dom {
	class element;
}

namespace Json {
	class Writer;
}

// Supported property types for serialization
enum class PropertyType : uint8_t
{
	Float,
	Int,
	Bool,
	String,
	Vec2
};

// Property metadata for editor UI and serialization
struct PropertyMeta
{
	std::string_view Name;           // Internal name (matches JSON key)
	std::string_view DisplayName;    // Human-readable name for editor
	std::string_view Tooltip;        // Editor tooltip/description
	PropertyType Type;
	
	// Numeric constraints for editor sliders/validation
	float Min = -FLT_MAX;
	float Max = FLT_MAX;
	
	// Serialization callbacks (operate on void* component pointer)
	std::function<void(void*, const simdjson::dom::element&)> Deserialize;
	std::function<void(const void*, Json::Writer&)> Serialize;
};

