#pragma once

#include <string_view>
#include <string>
#include <cstdint>
#include <memory>
#include <optional>

// Forward declarations
template<typename T> struct Vector2;
using Vec2 = Vector2<float>;

//=============================================================================
// IDeserializer
//
// Abstract interface for deserializing data from any format.
// Implementations can read from JSON, binary (networking), config files, etc.
//
// The interface uses a hierarchical model where objects and arrays can be
// navigated. Each method returns a default value if the key doesn't exist
// or the value cannot be converted to the requested type.
//=============================================================================
class IDeserializer
{
public:
	virtual ~IDeserializer() = default;

	// Check if a key exists
	virtual bool Has(std::string_view key) const = 0;

	// Read primitive values with defaults
	virtual float GetFloat(std::string_view key, float defaultValue = 0.0f) const = 0;
	virtual double GetDouble(std::string_view key, double defaultValue = 0.0) const = 0;
	virtual int32_t GetInt(std::string_view key, int32_t defaultValue = 0) const = 0;
	virtual int64_t GetInt64(std::string_view key, int64_t defaultValue = 0) const = 0;
	virtual bool GetBool(std::string_view key, bool defaultValue = false) const = 0;
	virtual std::string GetString(std::string_view key, std::string_view defaultValue = "") const = 0;
	virtual Vec2 GetVec2(std::string_view key, const Vec2& defaultValue) const = 0;

	// Navigate to nested object (returns nullptr if not found or not an object)
	virtual std::unique_ptr<IDeserializer> GetObject(std::string_view key) const = 0;

	// Get array size (returns 0 if not found or not an array)
	virtual size_t GetArraySize(std::string_view key) const = 0;

	// Get array element deserializer
	virtual std::unique_ptr<IDeserializer> GetArrayElement(std::string_view key, size_t index) const = 0;

	// Type-safe template accessor
	template<typename T>
	T Get(std::string_view key, const T& defaultValue) const;
};

// Template specializations for Get<T>
template<>
inline float IDeserializer::Get<float>(std::string_view key, const float& defaultValue) const {
	return GetFloat(key, defaultValue);
}

template<>
inline double IDeserializer::Get<double>(std::string_view key, const double& defaultValue) const {
	return GetDouble(key, defaultValue);
}

template<>
inline int IDeserializer::Get<int>(std::string_view key, const int& defaultValue) const {
	return GetInt(key, defaultValue);
}

template<>
inline int64_t IDeserializer::Get<int64_t>(std::string_view key, const int64_t& defaultValue) const {
	return GetInt64(key, defaultValue);
}

template<>
inline bool IDeserializer::Get<bool>(std::string_view key, const bool& defaultValue) const {
	return GetBool(key, defaultValue);
}

template<>
inline std::string IDeserializer::Get<std::string>(std::string_view key, const std::string& defaultValue) const {
	return GetString(key, defaultValue);
}

template<>
inline Vec2 IDeserializer::Get<Vec2>(std::string_view key, const Vec2& defaultValue) const {
	return GetVec2(key, defaultValue);
}

