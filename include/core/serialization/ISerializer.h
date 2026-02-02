#pragma once

#include <string_view>
#include <cstdint>

// Forward declarations
template<typename T> struct Vector2;
using Vec2 = Vector2<float>;

//=============================================================================
// ISerializer
//
// Abstract interface for serializing data to any format.
// Implementations can target JSON, binary (for networking), debug output, etc.
//
// The interface uses a hierarchical model where objects and arrays can be
// nested. Implementations handle format-specific details like delimiters,
// encoding, and structure.
//=============================================================================
class ISerializer
{
public:
	virtual ~ISerializer() = default;

	// Object structure
	virtual void BeginObject() = 0;
	virtual void BeginObject(std::string_view key) = 0;
	virtual void EndObject() = 0;

	// Array structure
	virtual void BeginArray(std::string_view key) = 0;
	virtual void EndArray() = 0;

	// Write key-value pairs
	virtual void Write(std::string_view key, float value) = 0;
	virtual void Write(std::string_view key, double value) = 0;
	virtual void Write(std::string_view key, int32_t value) = 0;
	virtual void Write(std::string_view key, int64_t value) = 0;
	virtual void Write(std::string_view key, bool value) = 0;
	virtual void Write(std::string_view key, std::string_view value) = 0;
	virtual void Write(std::string_view key, const Vec2& value) = 0;

	// Write array elements (no key)
	virtual void WriteValue(float value) = 0;
	virtual void WriteValue(int32_t value) = 0;
	virtual void WriteValue(std::string_view value) = 0;
};

