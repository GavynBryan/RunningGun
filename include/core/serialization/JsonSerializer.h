#pragma once

#include "ISerializer.h"
#include <core/math/Vec2.h>
#include <sstream>
#include <vector>
#include <string>

//=============================================================================
// JsonSerializer
//
// ISerializer implementation that produces JSON output.
// Uses a simple streaming approach for efficient serialization.
//=============================================================================
class JsonSerializer : public ISerializer
{
public:
	JsonSerializer() = default;

	// ISerializer interface
	void BeginObject() override {
		AppendCommaIfNeeded();
		m_Buffer << '{';
		m_StateStack.push_back(State::Object);
		m_NeedComma = false;
	}

	void BeginObject(std::string_view key) override {
		AppendCommaIfNeeded();
		WriteKey(key);
		m_Buffer << '{';
		m_StateStack.push_back(State::Object);
		m_NeedComma = false;
	}

	void EndObject() override {
		m_Buffer << '}';
		m_StateStack.pop_back();
		m_NeedComma = true;
	}

	void BeginArray(std::string_view key) override {
		AppendCommaIfNeeded();
		WriteKey(key);
		m_Buffer << '[';
		m_StateStack.push_back(State::Array);
		m_NeedComma = false;
	}

	void EndArray() override {
		m_Buffer << ']';
		m_StateStack.pop_back();
		m_NeedComma = true;
	}

	void Write(std::string_view key, float value) override {
		AppendCommaIfNeeded();
		WriteKey(key);
		m_Buffer << value;
		m_NeedComma = true;
	}

	void Write(std::string_view key, double value) override {
		AppendCommaIfNeeded();
		WriteKey(key);
		m_Buffer << value;
		m_NeedComma = true;
	}

	void Write(std::string_view key, int32_t value) override {
		AppendCommaIfNeeded();
		WriteKey(key);
		m_Buffer << value;
		m_NeedComma = true;
	}

	void Write(std::string_view key, int64_t value) override {
		AppendCommaIfNeeded();
		WriteKey(key);
		m_Buffer << value;
		m_NeedComma = true;
	}

	void Write(std::string_view key, bool value) override {
		AppendCommaIfNeeded();
		WriteKey(key);
		m_Buffer << (value ? "true" : "false");
		m_NeedComma = true;
	}

	void Write(std::string_view key, std::string_view value) override {
		AppendCommaIfNeeded();
		WriteKey(key);
		WriteString(value);
		m_NeedComma = true;
	}

	void Write(std::string_view key, const Vec2& value) override {
		AppendCommaIfNeeded();
		WriteKey(key);
		m_Buffer << '[' << value.x << ',' << value.y << ']';
		m_NeedComma = true;
	}

	void WriteValue(float value) override {
		AppendCommaIfNeeded();
		m_Buffer << value;
		m_NeedComma = true;
	}

	void WriteValue(int32_t value) override {
		AppendCommaIfNeeded();
		m_Buffer << value;
		m_NeedComma = true;
	}

	void WriteValue(std::string_view value) override {
		AppendCommaIfNeeded();
		WriteString(value);
		m_NeedComma = true;
	}

	// Get the serialized JSON string
	std::string ToString() const {
		return m_Buffer.str();
	}

	// Clear and reset for reuse
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

