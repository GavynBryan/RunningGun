#include <core/JsonValue.h>

const JsonValue JsonValue::Undefined;

JsonValue::JsonValue()
	: Value(Null{})
{
}

JsonValue::JsonValue(bool _value)
	: Value(_value)
{
}

JsonValue::JsonValue(int _value)
	: Value(static_cast<int64_t>(_value))
{
}

JsonValue::JsonValue(int64_t _value)
	: Value(_value)
{
}

JsonValue::JsonValue(double _value)
	: Value(_value)
{
}

JsonValue::JsonValue(const char* _value)
	: Value(std::string(_value))
{
}

JsonValue::JsonValue(std::string _value)
	: Value(std::move(_value))
{
}

JsonValue::JsonValue(Array _value)
	: Value(std::move(_value))
{
}

JsonValue::JsonValue(Object _value)
	: Value(std::move(_value))
{
}

bool JsonValue::IsNull() const
{
	return std::holds_alternative<Null>(Value);
}

bool JsonValue::IsBool() const
{
	return std::holds_alternative<bool>(Value);
}

bool JsonValue::IsInt() const
{
	return std::holds_alternative<int64_t>(Value);
}

bool JsonValue::IsDouble() const
{
	return std::holds_alternative<double>(Value);
}

bool JsonValue::IsNumber() const
{
	return IsInt() || IsDouble();
}

bool JsonValue::IsString() const
{
	return std::holds_alternative<std::string>(Value);
}

bool JsonValue::IsArray() const
{
	return std::holds_alternative<Array>(Value);
}

bool JsonValue::IsObject() const
{
	return std::holds_alternative<Object>(Value);
}

bool JsonValue::GetBool(bool _default) const
{
	if (auto* _val = std::get_if<bool>(&Value)) {
		return *_val;
	}
	return _default;
}

int64_t JsonValue::GetInt(int64_t _default) const
{
	if (auto* _val = std::get_if<int64_t>(&Value)) {
		return *_val;
	}
	if (auto* _val = std::get_if<double>(&Value)) {
		return static_cast<int64_t>(*_val);
	}
	return _default;
}

double JsonValue::GetDouble(double _default) const
{
	if (auto* _val = std::get_if<double>(&Value)) {
		return *_val;
	}
	if (auto* _val = std::get_if<int64_t>(&Value)) {
		return static_cast<double>(*_val);
	}
	return _default;
}

const std::string& JsonValue::GetString() const
{
	static const std::string EmptyString;
	if (auto* _val = std::get_if<std::string>(&Value)) {
		return *_val;
	}
	return EmptyString;
}

const JsonValue::Array& JsonValue::GetArray() const
{
	static const Array EmptyArray;
	if (auto* _val = std::get_if<Array>(&Value)) {
		return *_val;
	}
	return EmptyArray;
}

const JsonValue::Object& JsonValue::GetObject() const
{
	static const Object EmptyObject;
	if (auto* _val = std::get_if<Object>(&Value)) {
		return *_val;
	}
	return EmptyObject;
}

bool JsonValue::HasMember(const std::string& _key) const
{
	if (auto* _obj = std::get_if<Object>(&Value)) {
		return _obj->find(_key) != _obj->end();
	}
	return false;
}

const JsonValue& JsonValue::operator[](const std::string& _key) const
{
	if (auto* _obj = std::get_if<Object>(&Value)) {
		auto _it = _obj->find(_key);
		if (_it != _obj->end()) {
			return _it->second;
		}
	}
	return Undefined;
}

const JsonValue& JsonValue::operator[](size_t _index) const
{
	if (auto* _arr = std::get_if<Array>(&Value)) {
		if (_index < _arr->size()) {
			return (*_arr)[_index];
		}
	}
	return Undefined;
}

size_t JsonValue::Size() const
{
	if (auto* _arr = std::get_if<Array>(&Value)) {
		return _arr->size();
	}
	if (auto* _obj = std::get_if<Object>(&Value)) {
		return _obj->size();
	}
	return 0;
}
