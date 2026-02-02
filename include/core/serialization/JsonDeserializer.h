#pragma once

#include "IDeserializer.h"
#include <core/math/Vec2.h>
#include <simdjson.h>
#include <string>
#include <memory>

//=============================================================================
// JsonDeserializer
//
// IDeserializer implementation that reads from JSON using simdjson.
// Wraps a simdjson::dom::element and provides the abstract interface.
//=============================================================================
class JsonDeserializer : public IDeserializer
{
public:
	// Construct from a simdjson element
	explicit JsonDeserializer(simdjson::dom::element element)
		: m_Element(element)
	{
	}

	// Parse JSON string and create deserializer
	// Returns nullptr on parse error
	static std::unique_ptr<JsonDeserializer> Parse(const std::string& json) {
		static thread_local simdjson::dom::parser parser;
		auto result = parser.parse(simdjson::padded_string(json));
		if (result.error()) {
			return nullptr;
		}
		return std::make_unique<JsonDeserializer>(result.value());
	}

	// Load JSON from file and create deserializer
	// Returns nullptr on error
	static std::unique_ptr<JsonDeserializer> LoadFile(const std::string& path) {
		static thread_local simdjson::dom::parser parser;
		auto result = parser.load(path);
		if (result.error()) {
			return nullptr;
		}
		return std::make_unique<JsonDeserializer>(result.value());
	}

	// IDeserializer interface
	bool Has(std::string_view key) const override {
		return !m_Element[key].error();
	}

	float GetFloat(std::string_view key, float defaultValue = 0.0f) const override {
		auto result = m_Element[key].get_double();
		return result.error() ? defaultValue : static_cast<float>(result.value());
	}

	double GetDouble(std::string_view key, double defaultValue = 0.0) const override {
		auto result = m_Element[key].get_double();
		return result.error() ? defaultValue : result.value();
	}

	int32_t GetInt(std::string_view key, int32_t defaultValue = 0) const override {
		auto result = m_Element[key].get_int64();
		return result.error() ? defaultValue : static_cast<int32_t>(result.value());
	}

	int64_t GetInt64(std::string_view key, int64_t defaultValue = 0) const override {
		auto result = m_Element[key].get_int64();
		return result.error() ? defaultValue : result.value();
	}

	bool GetBool(std::string_view key, bool defaultValue = false) const override {
		auto result = m_Element[key].get_bool();
		return result.error() ? defaultValue : result.value();
	}

	std::string GetString(std::string_view key, std::string_view defaultValue = "") const override {
		auto result = m_Element[key].get_string();
		return result.error() ? std::string(defaultValue) : std::string(result.value());
	}

	Vec2 GetVec2(std::string_view key, const Vec2& defaultValue) const override {
		auto result = m_Element[key].get_array();
		if (result.error()) {
			return defaultValue;
		}

		auto arr = result.value();
		float x = defaultValue.x;
		float y = defaultValue.y;
		size_t idx = 0;

		for (auto val : arr) {
			if (idx == 0) {
				x = static_cast<float>(val.get_double().value_or(defaultValue.x));
			}
			if (idx == 1) {
				y = static_cast<float>(val.get_double().value_or(defaultValue.y));
			}
			idx++;
		}

		return Vec2(x, y);
	}

	std::unique_ptr<IDeserializer> GetObject(std::string_view key) const override {
		auto result = m_Element[key].get_object();
		if (result.error()) {
			return nullptr;
		}
		// Need to get the element, not the object view
		auto elem = m_Element[key];
		if (elem.error()) {
			return nullptr;
		}
		return std::make_unique<JsonDeserializer>(elem.value());
	}

	size_t GetArraySize(std::string_view key) const override {
		auto result = m_Element[key].get_array();
		if (result.error()) {
			return 0;
		}
		return result.value().size();
	}

	std::unique_ptr<IDeserializer> GetArrayElement(std::string_view key, size_t index) const override {
		auto result = m_Element[key].get_array();
		if (result.error()) {
			return nullptr;
		}

		auto arr = result.value();
		size_t idx = 0;
		for (auto elem : arr) {
			if (idx == index) {
				return std::make_unique<JsonDeserializer>(elem);
			}
			idx++;
		}

		return nullptr;
	}

	// Direct access to underlying element (for advanced usage)
	const simdjson::dom::element& GetElement() const {
		return m_Element;
	}

private:
	simdjson::dom::element m_Element;
};

