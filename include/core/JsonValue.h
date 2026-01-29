#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

class JsonValue
{
public:
	using Null = std::monostate;
	using Object = std::unordered_map<std::string, JsonValue>;
	using Array = std::vector<JsonValue>;
	using ValueType = std::variant<Null, bool, int64_t, double, std::string, Array, Object>;

	JsonValue();
	JsonValue(bool _value);
	JsonValue(int _value);
	JsonValue(int64_t _value);
	JsonValue(double _value);
	JsonValue(const char* _value);
	JsonValue(std::string _value);
	JsonValue(Array _value);
	JsonValue(Object _value);

	bool IsNull() const;
	bool IsBool() const;
	bool IsInt() const;
	bool IsDouble() const;
	bool IsNumber() const;
	bool IsString() const;
	bool IsArray() const;
	bool IsObject() const;

	bool GetBool(bool _default = false) const;
	int64_t GetInt(int64_t _default = 0) const;
	double GetDouble(double _default = 0.0) const;
	const std::string& GetString() const;
	const Array& GetArray() const;
	const Object& GetObject() const;

	bool HasMember(const std::string& _key) const;
	const JsonValue& operator[](const std::string& _key) const;
	const JsonValue& operator[](size_t _index) const;
	size_t Size() const;

	static const JsonValue Undefined;

private:
	ValueType Value;
};
