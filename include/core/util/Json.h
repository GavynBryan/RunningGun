#pragma once

#include <simdjson.h>
#include <string>
#include <sstream>
#include <vector>
#include <core/math/Vec2.h>

namespace Json {
	inline simdjson::dom::parser& GetParser() {
		static thread_local simdjson::dom::parser parser;
		return parser;
	}

	inline simdjson::simdjson_result<simdjson::dom::element> Parse(const std::string& _json) {
		return GetParser().parse(simdjson::padded_string(_json));
	}

	inline simdjson::simdjson_result<simdjson::dom::element> ParseFile(const std::string& _path) {
		return GetParser().load(_path);
	}

	// ============== READING (Deserialization) ==============

	inline double GetDouble(simdjson::dom::element _elem, std::string_view _key, double _default = 0.0) {
		auto _result = _elem[_key].get_double();
		return _result.error() ? _default : _result.value();
	}

	inline int64_t GetInt(simdjson::dom::element _elem, std::string_view _key, int64_t _default = 0) {
		auto _result = _elem[_key].get_int64();
		return _result.error() ? _default : _result.value();
	}

	inline std::string_view GetString(simdjson::dom::element _elem, std::string_view _key, std::string_view _default = "") {
		auto _result = _elem[_key].get_string();
		return _result.error() ? _default : _result.value();
	}

	inline bool GetBool(simdjson::dom::element _elem, std::string_view _key, bool _default = false) {
		auto _result = _elem[_key].get_bool();
		return _result.error() ? _default : _result.value();
	}

	inline Vec2 GetVec2(simdjson::dom::element _elem, std::string_view _key, Vec2 _default = Vec2(0.0f, 0.0f)) {
		auto _result = _elem[_key].get_array();
		if (_result.error()) return _default;
		auto arr = _result.value();
		float x = _default.x, y = _default.y;
		size_t idx = 0;
		for (auto val : arr) {
			if (idx == 0) x = static_cast<float>(val.get_double().value_or(_default.x));
			if (idx == 1) y = static_cast<float>(val.get_double().value_or(_default.y));
			idx++;
		}
		return Vec2(x, y);
	}

	// Type-safe template getters for use in macros
	template<typename T>
	T Get(simdjson::dom::element elem, std::string_view key, T defaultValue);

	template<>
	inline float Get<float>(simdjson::dom::element elem, std::string_view key, float defaultValue) {
		return static_cast<float>(GetDouble(elem, key, defaultValue));
	}

	template<>
	inline int Get<int>(simdjson::dom::element elem, std::string_view key, int defaultValue) {
		return static_cast<int>(GetInt(elem, key, defaultValue));
	}

	template<>
	inline bool Get<bool>(simdjson::dom::element elem, std::string_view key, bool defaultValue) {
		return GetBool(elem, key, defaultValue);
	}

	template<>
	inline std::string Get<std::string>(simdjson::dom::element elem, std::string_view key, std::string defaultValue) {
		return std::string(GetString(elem, key, defaultValue));
	}

	template<>
	inline Vec2 Get<Vec2>(simdjson::dom::element elem, std::string_view key, Vec2 defaultValue) {
		return GetVec2(elem, key, defaultValue);
	}

	// ============== WRITING (Serialization) ==============

	// Simple JSON writer for bidirectional serialization
	class Writer
	{
	public:
		Writer() = default;

		// Object/Array structure
		void BeginObject() {
			AppendCommaIfNeeded();
			m_Buffer << '{';
			m_StateStack.push_back(State::Object);
			m_NeedComma = false;
		}

		void BeginObject(std::string_view key) {
			AppendCommaIfNeeded();
			WriteKey(key);
			m_Buffer << '{';
			m_StateStack.push_back(State::Object);
			m_NeedComma = false;
		}

		void EndObject() {
			m_Buffer << '}';
			m_StateStack.pop_back();
			m_NeedComma = true;
		}

		void BeginArray(std::string_view key) {
			AppendCommaIfNeeded();
			WriteKey(key);
			m_Buffer << '[';
			m_StateStack.push_back(State::Array);
			m_NeedComma = false;
		}

		void EndArray() {
			m_Buffer << ']';
			m_StateStack.pop_back();
			m_NeedComma = true;
		}

		// Primitive writers
		void Write(std::string_view key, float value) {
			AppendCommaIfNeeded();
			WriteKey(key);
			m_Buffer << value;
			m_NeedComma = true;
		}

		void Write(std::string_view key, int value) {
			AppendCommaIfNeeded();
			WriteKey(key);
			m_Buffer << value;
			m_NeedComma = true;
		}

		void Write(std::string_view key, bool value) {
			AppendCommaIfNeeded();
			WriteKey(key);
			m_Buffer << (value ? "true" : "false");
			m_NeedComma = true;
		}

		void Write(std::string_view key, std::string_view value) {
			AppendCommaIfNeeded();
			WriteKey(key);
			WriteString(value);
			m_NeedComma = true;
		}

		void Write(std::string_view key, const std::string& value) {
			Write(key, std::string_view(value));
		}

		void Write(std::string_view key, const Vec2& value) {
			AppendCommaIfNeeded();
			WriteKey(key);
			m_Buffer << '[' << value.x << ',' << value.y << ']';
			m_NeedComma = true;
		}

		// Array element writers (no key)
		void WriteValue(float value) {
			AppendCommaIfNeeded();
			m_Buffer << value;
			m_NeedComma = true;
		}

		void WriteValue(int value) {
			AppendCommaIfNeeded();
			m_Buffer << value;
			m_NeedComma = true;
		}

		void WriteValue(std::string_view value) {
			AppendCommaIfNeeded();
			WriteString(value);
			m_NeedComma = true;
		}

		// Get final JSON string
		std::string ToString() const {
			return m_Buffer.str();
		}

		void Clear() {
			m_Buffer.str("");
			m_Buffer.clear();
			m_StateStack.clear();
			m_NeedComma = false;
		}

	private:
		enum class State { Object, Array };

		void WriteKey(std::string_view key) {
			m_Buffer << '"' << key << "\":";
		}

		void WriteString(std::string_view str) {
			m_Buffer << '"';
			for (char c : str) {
				switch (c) {
					case '"':  m_Buffer << "\\\""; break;
					case '\\': m_Buffer << "\\\\"; break;
					case '\n': m_Buffer << "\\n"; break;
					case '\r': m_Buffer << "\\r"; break;
					case '\t': m_Buffer << "\\t"; break;
					default:   m_Buffer << c; break;
				}
			}
			m_Buffer << '"';
		}

		void AppendCommaIfNeeded() {
			if (m_NeedComma) {
				m_Buffer << ',';
			}
		}

		std::ostringstream m_Buffer;
		std::vector<State> m_StateStack;
		bool m_NeedComma = false;
	};
}
